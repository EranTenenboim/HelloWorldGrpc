#ifndef HELLOWORLD_CLIENT_H
#define HELLOWORLD_CLIENT_H

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <mutex>

#include "proto/helloworld.grpc.pb.h"

namespace helloworld {

// Client communication service implementation
class ClientCommunicationServiceImpl final : public helloworld::ClientCommunication::Service {
 public:
  grpc::Status SendMessage(grpc::ServerContext* context,
                          const helloworld::ClientMessage* request,
                          helloworld::MessageResponse* reply) override;
  
  grpc::Status ReceiveMessage(grpc::ServerContext* context,
                             const helloworld::MessageRequest* request,
                             helloworld::ClientMessage* reply) override;

 private:
  std::vector<helloworld::ClientMessage> message_queue_;
  std::mutex message_mutex_;
};

// Client registry client
class ClientRegistryClient {
 public:
  explicit ClientRegistryClient(std::shared_ptr<grpc::Channel> channel);

  // Register this client with the registry
  bool RegisterClient(const std::string& client_id,
                      const std::string& client_address,
                      int32_t client_port) const;
  
  // Get client information by ID
  bool GetClient(const std::string& client_id,
                 std::string& address,
                 int32_t& port,
                 bool& online) const;
  
  // List all registered clients
  std::vector<std::tuple<std::string, std::string, int32_t, bool>> ListClients() const;
  
  // Unregister this client
  bool UnregisterClient(const std::string& client_id) const;

 private:
  std::unique_ptr<helloworld::ClientRegistry::Stub> stub_;
};

// Direct client-to-client communication
class ClientCommunicationClient {
 public:
  explicit ClientCommunicationClient(std::shared_ptr<grpc::Channel> channel);

  // Send message to another client
  bool SendMessage(const std::string& from_client_id,
                   const std::string& to_client_id,
                   const std::string& message_content) const;

 private:
  std::unique_ptr<helloworld::ClientCommunication::Stub> stub_;
};

// Main client class that combines registry and communication
class Client {
 public:
  Client(const std::string& registry_server_address,
         const std::string& client_id,
         const std::string& client_address,
         int32_t client_port);

  // Start the client (register and start listening)
  bool Start();
  
  // Send message to another client
  bool SendMessageToClient(const std::string& target_client_id, const std::string& message);
  
  // Get list of available clients
  std::vector<std::tuple<std::string, std::string, int32_t, bool>> GetAvailableClients();
  
  // Stop the client
  void Stop();

 private:
  std::string client_id_;
  std::string client_address_;
  int32_t client_port_;
  
  std::unique_ptr<ClientRegistryClient> registry_client_;
  std::unique_ptr<ClientCommunicationServiceImpl> communication_service_;
  std::unique_ptr<grpc::Server> communication_server_;
  std::thread server_thread_;
  
  bool running_;
  std::mutex running_mutex_;
};

// Client server functions
void RunClientCommunicationServer(const std::string& client_address, int32_t client_port);

}  // namespace helloworld

#endif  // HELLOWORLD_CLIENT_H
