#include "srv/server.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <memory>
#include <string>
#include <thread>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {
namespace {

// Test fixture for ClientRegistryService
class ClientRegistryServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    service_ = std::make_unique<ClientRegistryServiceImpl>();
  }

  std::unique_ptr<ClientRegistryServiceImpl> service_;
};

// Test client registration
TEST_F(ClientRegistryServiceTest, RegisterClient) {
  helloworld::ClientRegistration request;
  request.set_client_id("test_client");
  request.set_client_address("localhost");
  request.set_client_port(50052);
  
  helloworld::RegistrationResponse reply;
  grpc::ServerContext context;
  
  grpc::Status status = service_->RegisterClient(&context, &request, &reply);
  
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(reply.success());
  EXPECT_EQ(reply.message(), "Client registered successfully");
}

// Test duplicate client registration
TEST_F(ClientRegistryServiceTest, RegisterDuplicateClient) {
  // Register client first time
  helloworld::ClientRegistration request;
  request.set_client_id("duplicate_client");
  request.set_client_address("localhost");
  request.set_client_port(50052);
  
  helloworld::RegistrationResponse reply;
  grpc::ServerContext context;
  
  grpc::Status status1 = service_->RegisterClient(&context, &request, &reply);
  EXPECT_TRUE(status1.ok());
  EXPECT_TRUE(reply.success());
  
  // Try to register same client again
  grpc::Status status2 = service_->RegisterClient(&context, &request, &reply);
  EXPECT_TRUE(status2.ok());
  EXPECT_FALSE(reply.success());
  EXPECT_EQ(reply.message(), "Client ID already exists");
}

// Test get client
TEST_F(ClientRegistryServiceTest, GetClient) {
  // First register a client
  helloworld::ClientRegistration reg_request;
  reg_request.set_client_id("test_client");
  reg_request.set_client_address("localhost");
  reg_request.set_client_port(50052);
  
  helloworld::RegistrationResponse reg_reply;
  grpc::ServerContext reg_context;
  service_->RegisterClient(&reg_context, &reg_request, &reg_reply);
  
  // Now get the client
  helloworld::ClientLookup lookup_request;
  lookup_request.set_client_id("test_client");
  
  helloworld::ClientInfo client_info;
  grpc::ServerContext lookup_context;
  
  grpc::Status status = service_->GetClient(&lookup_context, &lookup_request, &client_info);
  
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(client_info.client_id(), "test_client");
  EXPECT_EQ(client_info.client_address(), "localhost");
  EXPECT_EQ(client_info.client_port(), 50052);
  EXPECT_TRUE(client_info.online());
}

// Test get non-existent client
TEST_F(ClientRegistryServiceTest, GetNonExistentClient) {
  helloworld::ClientLookup request;
  request.set_client_id("non_existent");
  
  helloworld::ClientInfo client_info;
  grpc::ServerContext context;
  
  grpc::Status status = service_->GetClient(&context, &request, &client_info);
  
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(client_info.client_id(), "non_existent");
  EXPECT_FALSE(client_info.online());
}

// Test list clients
TEST_F(ClientRegistryServiceTest, ListClients) {
  // Register multiple clients
  std::vector<std::string> client_ids = {"client1", "client2", "client3"};
  
  for (int i = 0; i < client_ids.size(); ++i) {
    helloworld::ClientRegistration request;
    request.set_client_id(client_ids[i]);
    request.set_client_address("localhost");
    request.set_client_port(50052 + i);
    
    helloworld::RegistrationResponse reply;
    grpc::ServerContext context;
    service_->RegisterClient(&context, &request, &reply);
  }
  
  // List all clients
  helloworld::ClientListRequest list_request;
  helloworld::ClientList client_list;
  grpc::ServerContext list_context;
  
  grpc::Status status = service_->ListClients(&list_context, &list_request, &client_list);
  
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(client_list.clients_size(), 3);
  
  // Check that all clients are in the list
  std::set<std::string> found_clients;
  for (const auto& client : client_list.clients()) {
    found_clients.insert(client.client_id());
  }
  
  for (const auto& client_id : client_ids) {
    EXPECT_TRUE(found_clients.find(client_id) != found_clients.end());
  }
}

// Test unregister client
TEST_F(ClientRegistryServiceTest, UnregisterClient) {
  // First register a client
  helloworld::ClientRegistration reg_request;
  reg_request.set_client_id("test_client");
  reg_request.set_client_address("localhost");
  reg_request.set_client_port(50052);
  
  helloworld::RegistrationResponse reg_reply;
  grpc::ServerContext reg_context;
  service_->RegisterClient(&reg_context, &reg_request, &reg_reply);
  
  // Now unregister the client
  helloworld::ClientUnregistration unreg_request;
  unreg_request.set_client_id("test_client");
  
  helloworld::UnregistrationResponse unreg_reply;
  grpc::ServerContext unreg_context;
  
  grpc::Status status = service_->UnregisterClient(&unreg_context, &unreg_request, &unreg_reply);
  
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(unreg_reply.success());
  EXPECT_EQ(unreg_reply.message(), "Client unregistered successfully");
}

// Test unregister non-existent client
TEST_F(ClientRegistryServiceTest, UnregisterNonExistentClient) {
  helloworld::ClientUnregistration request;
  request.set_client_id("non_existent");
  
  helloworld::UnregistrationResponse reply;
  grpc::ServerContext context;
  
  grpc::Status status = service_->UnregisterClient(&context, &request, &reply);
  
  EXPECT_TRUE(status.ok());
  EXPECT_FALSE(reply.success());
  EXPECT_EQ(reply.message(), "Client ID not found");
}

// Test concurrent client registrations
TEST_F(ClientRegistryServiceTest, ConcurrentClientRegistrations) {
  const int num_clients = 10;
  std::vector<std::thread> threads;
  std::atomic<int> success_count{0};
  
  for (int i = 0; i < num_clients; ++i) {
    threads.emplace_back([this, i, &success_count]() {
      helloworld::ClientRegistration request;
      request.set_client_id("concurrent_client_" + std::to_string(i));
      request.set_client_address("localhost");
      request.set_client_port(50052 + i);
      
      helloworld::RegistrationResponse reply;
      grpc::ServerContext context;
      
      grpc::Status status = service_->RegisterClient(&context, &request, &reply);
      if (status.ok() && reply.success()) {
        success_count++;
      }
    });
  }
  
  // Wait for all threads to complete
  for (auto& thread : threads) {
    thread.join();
  }
  
  // Verify all registrations succeeded
  EXPECT_EQ(success_count.load(), num_clients);
}

// Test empty client ID registration
TEST_F(ClientRegistryServiceTest, RegisterClientWithEmptyId) {
  helloworld::ClientRegistration request;
  request.set_client_id("");  // Empty ID
  request.set_client_address("localhost");
  request.set_client_port(50052);
  
  helloworld::RegistrationResponse reply;
  grpc::ServerContext context;
  
  grpc::Status status = service_->RegisterClient(&context, &request, &reply);
  
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(reply.success());
  EXPECT_EQ(reply.message(), "Client registered successfully");
}

// Test client registration with invalid port
TEST_F(ClientRegistryServiceTest, RegisterClientWithInvalidPort) {
  helloworld::ClientRegistration request;
  request.set_client_id("invalid_port_client");
  request.set_client_address("localhost");
  request.set_client_port(-1);  // Invalid port
  
  helloworld::RegistrationResponse reply;
  grpc::ServerContext context;
  
  grpc::Status status = service_->RegisterClient(&context, &request, &reply);
  
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(reply.success());
  EXPECT_EQ(reply.message(), "Client registered successfully");
}

}  // namespace
}  // namespace helloworld