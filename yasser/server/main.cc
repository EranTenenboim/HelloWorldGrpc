#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

#include "proto/hello.grpc.pb.h"

class GreeterServiceImpl final : public hello::Greeter::Service {
 public:
  grpc::Status SayHello(grpc::ServerContext* context, 
                       const hello::HelloRequest* request, 
                       hello::HelloReply* reply) override {
    (void)context;
    const std::string& msg = request->message();
    // Log incoming message from client
    std::cout << "Received from client: " << msg << "\n";

    // Expected format: "NAME: message". Extract NAME and respond with "Hello NAME".
    std::string::size_type colon_pos = msg.find(':');
    std::string name;
    if (colon_pos != std::string::npos) {
      name = msg.substr(0, colon_pos);
    } else {
      // Fallback when format isn't respected: use entire message as name
      name = msg;
    }

    std::string response = "Hello " + name;
    std::cout << "Responding to client: " << response << "\n";

    reply->set_message(response);
    return grpc::Status::OK;
  }
};

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  const std::string kServerAddress("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(kServerAddress, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if (!server) {
    return 1;
  }
  server->Wait();
  return 0;
}
