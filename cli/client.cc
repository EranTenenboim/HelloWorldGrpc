#include "client.h"

#include <iostream>

namespace helloworld {

GreeterClient::GreeterClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(helloworld::Greeter::NewStub(channel)) {}

std::string GreeterClient::SayHello(const std::string& user) const {
  // Data we are sending to the server.
  helloworld::HelloRequest request;
  request.set_name(user);

  // Container for the data we expect from the server.
  helloworld::HelloReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  // The actual RPC.
  grpc::Status status = stub_->SayHello(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return reply.message();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

}  // namespace helloworld


