#ifndef HELLOWORLD_SERVER_H
#define HELLOWORLD_SERVER_H

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {

// Client registry info
struct ClientRegistryInfo {
  std::string client_id;
  std::string address;
  int32_t port;
  bool online;
};

// Client registry service implementation
class ClientRegistryServiceImpl final : public helloworld::ClientRegistry::Service {
 public:
  grpc::Status RegisterClient(grpc::ServerContext* context,
                           const helloworld::ClientRegistration* request,
                           helloworld::RegistrationResponse* reply) override;
  
  grpc::Status GetClient(grpc::ServerContext* context,
                        const helloworld::ClientLookup* request,
                        helloworld::ClientInfo* reply) override;
  
  grpc::Status ListClients(grpc::ServerContext* context,
                          const helloworld::ClientListRequest* request,
                          helloworld::ClientList* reply) override;
  
  grpc::Status UnregisterClient(grpc::ServerContext* context,
                               const helloworld::ClientUnregistration* request,
                               helloworld::UnregistrationResponse* reply) override;

 private:
  std::map<std::string, ClientRegistryInfo> registered_clients_;
  std::mutex clients_mutex_;
};

// Server management functions
void RunServer();

}  // namespace helloworld

#endif  // HELLOWORLD_SERVER_H
