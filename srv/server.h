#ifndef HELLOWORLD_SERVER_H
#define HELLOWORLD_SERVER_H

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public helloworld::Greeter::Service {
 public:
  grpc::Status SayHello(grpc::ServerContext* context, 
                        const helloworld::HelloRequest* request,
                        helloworld::HelloReply* reply) override;
};

// Server management functions
void RunServer();

}  // namespace helloworld

#endif  // HELLOWORLD_SERVER_H
