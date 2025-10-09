#ifndef HELLOWORLD_CLIENT_H
#define HELLOWORLD_CLIENT_H

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {

class GreeterClient {
 public:
  explicit GreeterClient(std::shared_ptr<grpc::Channel> channel);

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) const;

 private:
  std::unique_ptr<helloworld::Greeter::Stub> stub_;
};

}  // namespace helloworld

#endif  // HELLOWORLD_CLIENT_H
