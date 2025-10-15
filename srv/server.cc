#include "server.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

namespace helloworld {

grpc::Status ClientRegistryServiceImpl::RegisterClient(grpc::ServerContext* context,
                                                      const helloworld::ClientRegistration* request,
                                                      helloworld::RegistrationResponse* reply) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  
  // Check if client already exists
  if (registered_clients_.find(request->client_id()) != registered_clients_.end()) {
    reply->set_success(false);
    reply->set_message("Client ID already exists");
    std::cout << "Client registration failed: ID " << request->client_id() << " already exists" << std::endl;
    return grpc::Status::OK;
  }
  
  // Register the client
  ClientRegistryInfo client_info;
  client_info.client_id = request->client_id();
  client_info.address = request->client_address();
  client_info.port = request->client_port();
  client_info.online = true;
  
  registered_clients_[request->client_id()] = client_info;
  
  reply->set_success(true);
  reply->set_message("Client registered successfully");
  std::cout << "Client " << request->client_id() << " registered at " 
            << request->client_address() << ":" << request->client_port() << std::endl;
  
  return grpc::Status::OK;
}

grpc::Status ClientRegistryServiceImpl::GetClient(grpc::ServerContext* context,
                                                  const helloworld::ClientLookup* request,
                                                  helloworld::ClientInfo* reply) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  
  auto it = registered_clients_.find(request->client_id());
  if (it == registered_clients_.end()) {
    reply->set_client_id(request->client_id());
    reply->set_client_address("");
    reply->set_client_port(0);
    reply->set_online(false);
    std::cout << "Client lookup failed: ID " << request->client_id() << " not found" << std::endl;
    return grpc::Status::OK;
  }
  
  const ClientRegistryInfo& client_info = it->second;
  reply->set_client_id(client_info.client_id);
  reply->set_client_address(client_info.address);
  reply->set_client_port(client_info.port);
  reply->set_online(client_info.online);
  
  std::cout << "Client lookup successful: " << client_info.client_id 
            << " at " << client_info.address << ":" << client_info.port << std::endl;
  
  return grpc::Status::OK;
}

grpc::Status ClientRegistryServiceImpl::ListClients(grpc::ServerContext* context,
                                                   const helloworld::ClientListRequest* request,
                                                   helloworld::ClientList* reply) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  
  for (const auto& [client_id, client_info] : registered_clients_) {
    helloworld::ClientInfo* client = reply->add_clients();
    client->set_client_id(client_info.client_id);
    client->set_client_address(client_info.address);
    client->set_client_port(client_info.port);
    client->set_online(client_info.online);
  }
  
  std::cout << "Listed " << registered_clients_.size() << " registered clients" << std::endl;
  
  return grpc::Status::OK;
}

grpc::Status ClientRegistryServiceImpl::UnregisterClient(grpc::ServerContext* context,
                                                         const helloworld::ClientUnregistration* request,
                                                         helloworld::UnregistrationResponse* reply) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  
  auto it = registered_clients_.find(request->client_id());
  if (it == registered_clients_.end()) {
    reply->set_success(false);
    reply->set_message("Client ID not found");
    std::cout << "Client unregistration failed: ID " << request->client_id() << " not found" << std::endl;
    return grpc::Status::OK;
  }
  
  registered_clients_.erase(it);
  
  reply->set_success(true);
  reply->set_message("Client unregistered successfully");
  std::cout << "Client " << request->client_id() << " unregistered" << std::endl;
  
  return grpc::Status::OK;
}

void RunServer() {
  const std::string server_address("0.0.0.0:50051");
  ClientRegistryServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register the client registry service
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Client Registry Server listening on " << server_address << std::endl;
  std::cout << "Clients can register and discover other clients" << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

}  // namespace helloworld


