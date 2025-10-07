#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "helloworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterServiceImpl final : public Greeter::Service {
 public:
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    (void)context;
    std::string prefix = "Hello ";
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

static void RunServer(const std::string& server_address) {
  GreeterServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  const std::string server_address = "0.0.0.0:50051";
  RunServer(server_address);
  return 0;
}


