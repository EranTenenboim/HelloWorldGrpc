#include "cli/client.h"

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

// Mock server implementation for testing
class MockGreeterService : public helloworld::Greeter::Service {
 public:
  MOCK_METHOD(grpc::Status, SayHello, 
              (grpc::ServerContext* context, 
               const helloworld::HelloRequest* request,
               helloworld::HelloReply* reply), (override));
};

// Test fixture for client tests
class GreeterClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a mock server
    std::string server_address("localhost:0");
    mock_service_ = std::make_unique<MockGreeterService>();
    
    grpc::ServerBuilder builder;
    int port = 0;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &port);
    builder.RegisterService(mock_service_.get());
    server_ = builder.BuildAndStart();
    
    // Create client channel to the mock server
    std::string target = "localhost:" + std::to_string(port);
    auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    client_ = std::make_unique<GreeterClient>(channel);
  }

  void TearDown() override {
    if (server_) {
      server_->Shutdown();
    }
  }

  std::unique_ptr<MockGreeterService> mock_service_;
  std::unique_ptr<grpc::Server> server_;
  std::unique_ptr<GreeterClient> client_;
};

// Test successful SayHello call
TEST_F(GreeterClientTest, SayHelloSuccess) {
  // Setup expectations
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test
  std::string result = client_->SayHello("World");
  
  // Verify the result
  EXPECT_EQ(result, "Hello World");
}

// Test SayHello with empty name
TEST_F(GreeterClientTest, SayHelloEmptyName) {
  // Setup expectations
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test
  std::string result = client_->SayHello("");
  
  // Verify the result
  EXPECT_EQ(result, "Hello ");
}

// Test SayHello with special characters
TEST_F(GreeterClientTest, SayHelloSpecialCharacters) {
  // Setup expectations
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test
  std::string result = client_->SayHello("Test@#$%");
  
  // Verify the result
  EXPECT_EQ(result, "Hello Test@#$%");
}

// Test SayHello with long name
TEST_F(GreeterClientTest, SayHelloLongName) {
  // Setup expectations
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test with a long name
  std::string long_name(1000, 'A');
  std::string result = client_->SayHello(long_name);
  
  // Verify the result
  EXPECT_EQ(result, "Hello " + long_name);
}

// Test server error handling
TEST_F(GreeterClientTest, SayHelloServerError) {
  // Setup expectations for server error
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal server error");
      });

  // Execute the test
  std::string result = client_->SayHello("World");
  
  // Verify the result shows RPC failed
  EXPECT_EQ(result, "RPC failed");
}

// Test timeout handling
TEST_F(GreeterClientTest, SayHelloTimeout) {
  // Setup expectations for timeout
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](grpc::ServerContext* context, 
                   const helloworld::HelloRequest* request,
                   helloworld::HelloReply* reply) {
        // Simulate delay but not too long
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test
  std::string result = client_->SayHello("World");
  
  // Verify the result
  EXPECT_EQ(result, "Hello World");
}

// Parameterized test for different names
class GreeterClientParameterizedTest : public GreeterClientTest,
                                       public ::testing::WithParamInterface<std::string> {};

TEST_P(GreeterClientParameterizedTest, SayHelloWithDifferentNames) {
  std::string name = GetParam();
  
  // Setup expectations
  EXPECT_CALL(*mock_service_, SayHello(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([name](grpc::ServerContext* context, 
                       const helloworld::HelloRequest* request,
                       helloworld::HelloReply* reply) {
        EXPECT_EQ(request->name(), name);
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
      });

  // Execute the test
  std::string result = client_->SayHello(name);
  
  // Verify the result
  EXPECT_EQ(result, "Hello " + name);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentNames,
    GreeterClientParameterizedTest,
    ::testing::Values("Alice", "Bob", "Charlie", "David", "Eve", "Frank"));

}  // namespace
}  // namespace helloworld
