#include "client.h"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>

int main(const int argc, const char* const argv[]) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  std::string target_str;
  std::string user("world");
  if (argc >= 2) {
    target_str = argv[1];
  } else {
    target_str = "localhost:50051";
  }
  if (argc >= 3) {
    user = argv[2];
  }
  const helloworld::GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  const std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
