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

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {
namespace {

// Integration test fixture
class IntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Start server in a separate thread
    server_thread_ = std::thread([this]() {
      const std::string server_address("localhost:0");
      service_ = std::make_unique<GreeterServiceImpl>();
      
      grpc::ServerBuilder builder;
      int port = 0;
      builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &port);
      builder.RegisterService(service_.get());
      server_ = builder.BuildAndStart();
      
      server_port_ = port;
      server_ready_ = true;
      
      // Wait for server to be ready
      //std::unique_lock<std::mutex> lock(server_mutex_);
      //server_cv_.wait(lock, [this] { return server_ready_; });
      server_cv_.notify_all();
      server_->Wait();
    });
    
    // Wait for server to be ready
    std::unique_lock<std::mutex> lock(server_mutex_);
    server_cv_.wait(lock, [this] { return server_ready_; });
    
    // Create client
    std::string target = "localhost:" + std::to_string(server_port_);
    auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    client_ = std::make_unique<GreeterClient>(channel);
  }

  void TearDown() override {
    if (server_) {
      server_->Shutdown();
    }
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
  }

  std::unique_ptr<GreeterServiceImpl> service_;
  std::unique_ptr<grpc::Server> server_;
  std::unique_ptr<GreeterClient> client_;
  std::thread server_thread_;
  int server_port_ = 0;
  bool server_ready_ = false;
  std::mutex server_mutex_;
  std::condition_variable server_cv_;
};

// Test basic client-server communication
TEST_F(IntegrationTest, BasicCommunication) {
  std::string result = client_->SayHello("World");
  EXPECT_EQ(result, "Hello World");
}

// Test multiple requests
TEST_F(IntegrationTest, MultipleRequests) {
  std::vector<std::string> names = {"Alice", "Bob", "Charlie", "David", "Eve"};
  
  for (const auto& name : names) {
    std::string result = client_->SayHello(name);
    EXPECT_EQ(result, "Hello " + name);
  }
}

// Test concurrent requests
TEST_F(IntegrationTest, ConcurrentRequests) {
  const int num_threads = 5;
  const int requests_per_thread = 10;
  std::vector<std::thread> threads;
  std::atomic<int> success_count{0};
  
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([this, &success_count, requests_per_thread, i]() {
      for (int j = 0; j < requests_per_thread; ++j) {
        std::string name = "Thread" + std::to_string(i) + "_Request" + std::to_string(j);
        std::string result = client_->SayHello(name);
        if (result == "Hello " + name) {
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

// Test empty name
TEST_F(IntegrationTest, EmptyName) {
  std::string result = client_->SayHello("");
  EXPECT_EQ(result, "Hello ");
}

// Test special characters
TEST_F(IntegrationTest, SpecialCharacters) {
  std::string result = client_->SayHello("Test@#$%");
  EXPECT_EQ(result, "Hello Test@#$%");
}

// Test long name
TEST_F(IntegrationTest, LongName) {
  std::string long_name(1000, 'A');
  std::string result = client_->SayHello(long_name);
  EXPECT_EQ(result, "Hello " + long_name);
}

// Test Unicode characters
TEST_F(IntegrationTest, UnicodeCharacters) {
  std::string result = client_->SayHello("世界");
  EXPECT_EQ(result, "Hello 世界");
}

// Test whitespace handling
TEST_F(IntegrationTest, WhitespaceHandling) {
  std::string result = client_->SayHello("  Test  ");
  EXPECT_EQ(result, "Hello   Test  ");
}

// Test newline handling
TEST_F(IntegrationTest, NewlineHandling) {
  std::string result = client_->SayHello("Test\nName");
  EXPECT_EQ(result, "Hello Test\nName");
}

// Parameterized test for different names
class IntegrationParameterizedTest : public IntegrationTest,
                                     public ::testing::WithParamInterface<std::string> {};

TEST_P(IntegrationParameterizedTest, SayHelloWithDifferentNames) {
  std::string name = GetParam();
  std::string result = client_->SayHello(name);
  EXPECT_EQ(result, "Hello " + name);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentNames,
    IntegrationParameterizedTest,
    ::testing::Values("Alice", "Bob", "Charlie", "David", "Eve", "Frank", 
                      "John Doe", "Jane Smith", "Test User", "世界", "Тест"));

// Test server restart scenario
TEST_F(IntegrationTest, ServerRestart) {
  // First request should work
  std::string result1 = client_->SayHello("World");
  EXPECT_EQ(result1, "Hello World");
  
  // Restart server
  if (server_) {
    server_->Shutdown();
  }
  if (server_thread_.joinable()) {
    server_thread_.join();
  }
  
  // Start new server
  server_thread_ = std::thread([this]() {
    const std::string server_address("localhost:0");
    service_ = std::make_unique<GreeterServiceImpl>();
    
    grpc::ServerBuilder builder;
    int port = 0;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &port);
    builder.RegisterService(service_.get());
    server_ = builder.BuildAndStart();
    
    server_port_ = port;
    server_ready_ = true;
    
    std::unique_lock<std::mutex> lock(server_mutex_);
    server_cv_.wait(lock, [this] { return server_ready_; });
    
    server_->Wait();
  });
  
  // Wait for new server to be ready
  std::unique_lock<std::mutex> lock(server_mutex_);
  server_cv_.wait(lock, [this] { return server_ready_; });
  
  // Create new client
  std::string target = "localhost:" + std::to_string(server_port_);
  auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
  client_ = std::make_unique<GreeterClient>(channel);
  
  // Second request should work
  std::string result2 = client_->SayHello("World");
  EXPECT_EQ(result2, "Hello World");
}

}  // namespace
}  // namespace helloworld
