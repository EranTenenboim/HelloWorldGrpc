#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient {
 public:
  explicit GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& user) {
    HelloRequest request;
    request.set_name(user);

    HelloReply reply;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &reply);
    if (!status.ok()) {
      return std::string("RPC failed: ") + status.error_message();
    }
    return reply.message();
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  const std::string target_str = (argc > 1) ? argv[1] : "localhost:50051";
  const std::string user = (argc > 2) ? argv[2] : "world";

  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::cout << greeter.SayHello(user) << std::endl;
  return 0;
}


