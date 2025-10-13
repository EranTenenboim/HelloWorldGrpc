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
#include <atomic>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {
namespace {

// Test fixture for server tests
class GreeterServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    service_ = std::make_unique<GreeterServiceImpl>();
  }

  std::unique_ptr<GreeterServiceImpl> service_;
};

// Test successful SayHello service method
TEST_F(GreeterServiceTest, SayHelloSuccess) {
  // Create test request
  helloworld::HelloRequest request;
  request.set_name("World");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello World");
}

// Test SayHello with empty name
TEST_F(GreeterServiceTest, SayHelloEmptyName) {
  // Create test request with empty name
  helloworld::HelloRequest request;
  request.set_name("");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello ");
}

// Test SayHello with special characters
TEST_F(GreeterServiceTest, SayHelloSpecialCharacters) {
  // Create test request with special characters
  helloworld::HelloRequest request;
  request.set_name("Test@#$%");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello Test@#$%");
}

// Test SayHello with long name
TEST_F(GreeterServiceTest, SayHelloLongName) {
  // Create test request with long name
  std::string long_name(1000, 'A');
  helloworld::HelloRequest request;
  request.set_name(long_name);
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello " + long_name);
}

// Test SayHello with Unicode characters
TEST_F(GreeterServiceTest, SayHelloUnicode) {
  // Create test request with Unicode characters
  helloworld::HelloRequest request;
  request.set_name("世界");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello 世界");
}

// Test SayHello with whitespace
TEST_F(GreeterServiceTest, SayHelloWhitespace) {
  // Create test request with whitespace
  helloworld::HelloRequest request;
  request.set_name("  Test  ");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello   Test  ");
}

// Test SayHello with newlines
TEST_F(GreeterServiceTest, SayHelloNewlines) {
  // Create test request with newlines
  helloworld::HelloRequest request;
  request.set_name("Test\nName");
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello Test\nName");
}

// Test null request handling
TEST_F(GreeterServiceTest, DISABLED_SayHelloNullRequest) {
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method with null request
  grpc::Status status = service_->SayHello(&context, nullptr, &reply);
  
  // Verify the result - should handle gracefully
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello ");
}

// Test null reply handling
TEST_F(GreeterServiceTest, DISABLED_SayHelloNullReply) {
  // Create test request
  helloworld::HelloRequest request;
  request.set_name("World");
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method with null reply
  grpc::Status status = service_->SayHello(&context, &request, nullptr);
  
  // Verify the result - should handle gracefully
  EXPECT_TRUE(status.ok());
}

// Parameterized test for different names
class GreeterServiceParameterizedTest : public GreeterServiceTest,
                                        public ::testing::WithParamInterface<std::string> {};

TEST_P(GreeterServiceParameterizedTest, SayHelloWithDifferentNames) {
  std::string name = GetParam();
  
  // Create test request
  helloworld::HelloRequest request;
  request.set_name(name);
  
  // Create response object
  helloworld::HelloReply reply;
  
  // Create server context
  grpc::ServerContext context;
  
  // Call the service method
  grpc::Status status = service_->SayHello(&context, &request, &reply);
  
  // Verify the result
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(reply.message(), "Hello " + name);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentNames,
    GreeterServiceParameterizedTest,
    ::testing::Values("Alice", "Bob", "Charlie", "David", "Eve", "Frank", 
                      "John Doe", "Jane Smith", "Test User"));

// Test concurrent access to the service
TEST_F(GreeterServiceTest, ConcurrentAccess) {
  const int num_threads = 10;
  const int requests_per_thread = 100;
  std::vector<std::thread> threads;
  std::atomic<int> success_count{0};
  
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([this, &success_count, requests_per_thread, i]() {
      for (int j = 0; j < requests_per_thread; ++j) {
        helloworld::HelloRequest request;
        request.set_name("Thread" + std::to_string(i) + "_Request" + std::to_string(j));
        
        helloworld::HelloReply reply;
        grpc::ServerContext context;
        
        grpc::Status status = service_->SayHello(&context, &request, &reply);
        if (status.ok() && reply.message() == "Hello Thread" + std::to_string(i) + "_Request" + std::to_string(j)) {
          success_count++;
        }
      }
    });
  }
  
  // Wait for all threads to complete
  for (auto& thread : threads) {
    thread.join();
  }
  
  // Verify all requests succeeded
  EXPECT_EQ(success_count.load(), num_threads * requests_per_thread);
}

}  // namespace
}  // namespace helloworld
