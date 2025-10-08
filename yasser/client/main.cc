#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "proto/hello.grpc.pb.h"

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  auto channel = grpc::CreateChannel("localhost:50051", 
                                   grpc::InsecureChannelCredentials());
  std::unique_ptr<hello::Greeter::Stub> stub = 
      hello::Greeter::NewStub(channel);

  // Pick a random client name for this instance
  static const std::vector<std::string> kCandidateNames = {
      "Adam", "Bella", "Charlie", "Diana", "Ethan", "Fiona",
      "George", "Hannah", "Ivan", "Julia", "Kevin", "Luna",
  };
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, kCandidateNames.size() - 1);
  const std::string client_name = kCandidateNames[dist(gen)];

  hello::HelloRequest request;
  request.set_message(client_name + ": Hello Server");
  std::cout << "Sending to server: " << request.message() << "\n";

  hello::HelloReply reply;
  grpc::ClientContext context;
  grpc::Status status = stub->SayHello(&context, request, &reply);
  std::cout << "Sent:... " << "\n";
  
  if (!status.ok()) {
    std::cerr << "RPC failed: " << status.error_message() << "\n";
    return 1;
  }

  std::cout << "Received from server: " << reply.message() << "\n";
  return 0;
}
