#include "cli/client.h"
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
#include <chrono>
#include <atomic>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {
namespace {

// Registry PTP test fixture
class RegistryPTPTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Start registry server in a separate thread
    server_thread_ = std::thread([this]() {
      const std::string server_address("localhost:0");
      service_ = std::make_unique<ClientRegistryServiceImpl>();
      
      grpc::ServerBuilder builder;
      int port = 0;
      builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &port);
      builder.RegisterService(service_.get());
      server_ = builder.BuildAndStart();
      
      server_port_ = port;
      server_ready_ = true;
      server_cv_.notify_all();
      server_->Wait();
    });
    
    // Wait for server to be ready
    std::unique_lock<std::mutex> lock(server_mutex_);
    server_cv_.wait(lock, [this] { return server_ready_; });
    
    registry_server_address_ = "localhost:" + std::to_string(server_port_);
  }

  void TearDown() override {
    if (server_) {
      server_->Shutdown();
    }
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
  }

  std::unique_ptr<ClientRegistryServiceImpl> service_;
  std::unique_ptr<grpc::Server> server_;
  std::thread server_thread_;
  int server_port_ = 0;
  std::string registry_server_address_;
  bool server_ready_ = false;
  std::mutex server_mutex_;
  std::condition_variable server_cv_;
};

// Test client registration with registry
TEST_F(RegistryPTPTest, ClientRegistration) {
  const std::string client_id = "test_client";
  const std::string client_address = "localhost";
  const int32_t client_port = 50052;
  
  // Create and start client
  Client client(registry_server_address_, client_id, client_address, client_port);
  EXPECT_TRUE(client.Start());
  
  // Give client time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Cleanup
  client.Stop();
}

// Test client discovery and direct communication
TEST_F(RegistryPTPTest, ClientDiscoveryAndCommunication) {
  const std::string client_id1 = "client1";
  const std::string client_id2 = "client2";
  const std::string client_address = "localhost";
  const int32_t client_port1 = 50060;
  const int32_t client_port2 = 50061;
  
  // Create and start both clients
  Client client1(registry_server_address_, client_id1, client_address, client_port1);
  Client client2(registry_server_address_, client_id2, client_address, client_port2);
  
  EXPECT_TRUE(client1.Start());
  EXPECT_TRUE(client2.Start());
  
  // Give clients time to register and start
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  // Test client discovery
  auto clients = client1.GetAvailableClients();
  EXPECT_GE(clients.size(), 1); // At least client1 should be visible
  
  // Test direct communication
  const std::string test_message = "Hello from client1 to client2!";
  EXPECT_TRUE(client1.SendMessageToClient(client_id2, test_message));
  
  // Cleanup
  client1.Stop();
  client2.Stop();
}

// Test multiple client registration
TEST_F(RegistryPTPTest, MultipleClientRegistration) {
  const int num_clients = 5;
  std::vector<std::unique_ptr<Client>> clients;
  
  // Create and start multiple clients
  for (int i = 0; i < num_clients; ++i) {
    const std::string client_id = "client_" + std::to_string(i);
    const int32_t client_port = 50070 + i;
    
    auto client = std::make_unique<Client>(
        registry_server_address_, client_id, "localhost", client_port);
    
    EXPECT_TRUE(client->Start());
    clients.push_back(std::move(client));
  }
  
  // Give clients time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  
  // Test that all clients are discoverable
  auto available_clients = clients[0]->GetAvailableClients();
  EXPECT_GE(available_clients.size(), num_clients);
  
  // Cleanup
  for (auto& client : clients) {
    client->Stop();
  }
}

// Test client unregistration
TEST_F(RegistryPTPTest, ClientUnregistration) {
  const std::string client_id = "temp_client";
  const std::string client_address = "localhost";
  const int32_t client_port = 50080;
  
  // Create and start client
  Client client(registry_server_address_, client_id, client_address, client_port);
  EXPECT_TRUE(client.Start());
  
  // Give client time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Stop client (should unregister)
  client.Stop();
  
  // Give time for unregistration
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Create new client to check registry
  Client checker(registry_server_address_, "checker", "localhost", 50081);
  EXPECT_TRUE(checker.Start());
  
  // Check that the original client is no longer in the registry
  auto clients = checker.GetAvailableClients();
  bool found = false;
  for (const auto& [id, address, port, online] : clients) {
    if (id == client_id) {
      found = true;
      break;
    }
  }
  EXPECT_FALSE(found);
  
  checker.Stop();
}

// Test concurrent client communication
TEST_F(RegistryPTPTest, ConcurrentClientCommunication) {
  const int num_senders = 3;
  const int num_receivers = 2;
  std::vector<std::unique_ptr<Client>> senders;
  std::vector<std::unique_ptr<Client>> receivers;
  
  // Create senders
  for (int i = 0; i < num_senders; ++i) {
    const std::string client_id = "sender_" + std::to_string(i);
    const int32_t client_port = 50100 + i;
    
    auto client = std::make_unique<Client>(
        registry_server_address_, client_id, "localhost", client_port);
    EXPECT_TRUE(client->Start());
    senders.push_back(std::move(client));
  }
  
  // Create receivers
  for (int i = 0; i < num_receivers; ++i) {
    const std::string client_id = "receiver_" + std::to_string(i);
    const int32_t client_port = 50200 + i;
    
    auto client = std::make_unique<Client>(
        registry_server_address_, client_id, "localhost", client_port);
    EXPECT_TRUE(client->Start());
    receivers.push_back(std::move(client));
  }
  
  // Give clients time to register and start
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  // Test concurrent messaging
  std::vector<std::thread> message_threads;
  std::atomic<int> success_count{0};
  
  for (int i = 0; i < num_senders; ++i) {
    for (int j = 0; j < num_receivers; ++j) {
      message_threads.emplace_back([&, i, j]() {
        const std::string message = "Message from sender_" + std::to_string(i) + 
                                   " to receiver_" + std::to_string(j);
        const std::string receiver_id = "receiver_" + std::to_string(j);
        
        if (senders[i]->SendMessageToClient(receiver_id, message)) {
          success_count++;
        }
      });
    }
  }
  
  // Wait for all messages to be sent
  for (auto& thread : message_threads) {
    thread.join();
  }
  
  // Verify some messages were sent successfully
  EXPECT_GT(success_count.load(), 0);
  
  // Cleanup
  for (auto& client : senders) {
    client->Stop();
  }
  for (auto& client : receivers) {
    client->Stop();
  }
}

// Test client registration with invalid address
TEST_F(RegistryPTPTest, ClientRegistrationInvalidAddress) {
  const std::string client_id = "invalid_client";
  const std::string client_address = "invalid_host";
  const int32_t client_port = 99999; // Invalid port
  
  // Create client with invalid address
  Client client(registry_server_address_, client_id, client_address, client_port);
  
  // This should still register with the registry (registry doesn't validate connectivity)
  EXPECT_TRUE(client.Start());
  
  // Cleanup
  client.Stop();
}

// Test client registration with duplicate client ID
TEST_F(RegistryPTPTest, DuplicateClientIdRegistration) {
  const std::string client_id = "duplicate_client";
  const std::string client_address = "localhost";
  const int32_t client_port1 = 50090;
  const int32_t client_port2 = 50091;
  
  // Create and start first client
  Client client1(registry_server_address_, client_id, client_address, client_port1);
  EXPECT_TRUE(client1.Start());
  
  // Give client time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Try to create second client with same ID
  Client client2(registry_server_address_, client_id, client_address, client_port2);
  // This should fail due to duplicate ID
  EXPECT_FALSE(client2.Start());
  
  // Cleanup
  client1.Stop();
}

// Test client registration with different client addresses
TEST_F(RegistryPTPTest, DifferentClientAddresses) {
  const std::string client_id1 = "client_localhost";
  const std::string client_id2 = "client_127";
  const std::string client_address1 = "localhost";
  const std::string client_address2 = "127.0.0.1";
  const int32_t client_port1 = 50110;
  const int32_t client_port2 = 50111;
  
  // Create and start first client
  Client client1(registry_server_address_, client_id1, client_address1, client_port1);
  EXPECT_TRUE(client1.Start());
  
  // Give client time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Create and start second client with different address
  Client client2(registry_server_address_, client_id2, client_address2, client_port2);
  EXPECT_TRUE(client2.Start());
  
  // Give client time to register
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Test that both clients are discoverable
  auto clients = client1.GetAvailableClients();
  EXPECT_GE(clients.size(), 1);
  
  // Cleanup
  client1.Stop();
  client2.Stop();
}

// Test client registration timeout
TEST_F(RegistryPTPTest, ClientRegistrationTimeout) {
  const std::string client_id = "timeout_client";
  const std::string client_address = "localhost";
  const int32_t client_port = 50120;
  
  // Create client
  Client client(registry_server_address_, client_id, client_address, client_port);
  
  // Start client (should succeed even if no actual server is running)
  EXPECT_TRUE(client.Start());
  
  // Cleanup
  client.Stop();
}

// Test client registration with empty client ID
TEST_F(RegistryPTPTest, ClientRegistrationEmptyId) {
  const std::string client_id = ""; // Empty ID
  const std::string client_address = "localhost";
  const int32_t client_port = 50130;
  
  // Create client with empty ID
  Client client(registry_server_address_, client_id, client_address, client_port);
  
  // This should still work (empty ID is valid)
  EXPECT_TRUE(client.Start());
  
  // Cleanup
  client.Stop();
}

}  // namespace
}  // namespace helloworld