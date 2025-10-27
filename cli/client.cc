#include "client.h"

#include <iostream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <memory>
#include <string>
#include <chrono>

namespace helloworld {

// Client communication service implementation
grpc::Status ClientCommunicationServiceImpl::SendMessage(grpc::ServerContext* context,
                                                        const helloworld::ClientMessage* request,
                                                        helloworld::MessageResponse* reply) {
  std::lock_guard<std::mutex> lock(message_mutex_);
  
  // Store the message in the queue
  message_queue_.push_back(*request);
 
  std::cout << "["<< request->timestamp() << "] " << request->from_client_id() << ": " << request->message_content() << std::endl;
  std::cout.flush();
  
  reply->set_success(true);
  
  
  return grpc::Status::OK;
}

grpc::Status ClientCommunicationServiceImpl::ReceiveMessage(grpc::ServerContext* context,
                                                           const helloworld::MessageRequest* request,
                                                           helloworld::ClientMessage* reply) {
  std::lock_guard<std::mutex> lock(message_mutex_);
  
  if (message_queue_.empty()) {
    // No messages available
    reply->set_from_client_id("");
    reply->set_to_client_id("");
    reply->set_message_content("");
    reply->set_timestamp("");
    return grpc::Status::OK;
  }
  
  // Return the first message and remove it from queue
  *reply = message_queue_.front();
  message_queue_.erase(message_queue_.begin());
  
  return grpc::Status::OK;
}

std::vector<helloworld::ClientMessage> ClientCommunicationServiceImpl::GetMessageQueue() const {
  std::lock_guard<std::mutex> lock(message_mutex_);
  return message_queue_;
}

// Client registry client implementation
ClientRegistryClient::ClientRegistryClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(helloworld::ClientRegistry::NewStub(channel)) {}

bool ClientRegistryClient::RegisterClient(const std::string& client_id,
                                          const std::string& client_address,
                                          int32_t client_port) const {
  helloworld::ClientRegistration request;
  request.set_client_id(client_id);
  request.set_client_address(client_address);
  request.set_client_port(client_port);
  
  helloworld::RegistrationResponse reply;
  grpc::ClientContext context;
  
  grpc::Status status = stub_->RegisterClient(&context, request, &reply);
  
  if (status.ok() && reply.success()) {
    std::cout << "Successfully registered with registry: " << reply.message() << std::endl;
    return true;
  } else {
    std::cout << "Failed to register with registry: " << reply.message() << std::endl;
    return false;
  }
}

bool ClientRegistryClient::GetClient(const std::string& client_id,
                                     std::string& address,
                                     int32_t& port,
                                     bool& online) const {
  helloworld::ClientLookup request;
  request.set_client_id(client_id);
  
  helloworld::ClientInfo reply;
  grpc::ClientContext context;
  
  grpc::Status status = stub_->GetClient(&context, request, &reply);
  
  if (status.ok()) {
    address = reply.client_address();
    port = reply.client_port();
    online = reply.online();
    return true;
  } else {
    std::cout << "Failed to get client info: " << status.error_message() << std::endl;
    return false;
  }
}

std::vector<std::tuple<std::string, std::string, int32_t, bool>> ClientRegistryClient::ListClients() const {
  helloworld::ClientListRequest request;
  helloworld::ClientList reply;
  grpc::ClientContext context;
  
  grpc::Status status = stub_->ListClients(&context, request, &reply);
  
  std::vector<std::tuple<std::string, std::string, int32_t, bool>> clients;
  
  if (status.ok()) {
    for (const auto& client : reply.clients()) {
      clients.emplace_back(client.client_id(), client.client_address(), 
                          client.client_port(), client.online());
    }
  } else {
    std::cout << "Failed to list clients: " << status.error_message() << std::endl;
  }
  
  return clients;
}

bool ClientRegistryClient::UnregisterClient(const std::string& client_id) const {
  helloworld::ClientUnregistration request;
  request.set_client_id(client_id);
  
  helloworld::UnregistrationResponse reply;
  grpc::ClientContext context;
  
  grpc::Status status = stub_->UnregisterClient(&context, request, &reply);
  
  if (status.ok() && reply.success()) {
    std::cout << "Successfully unregistered: " << reply.message() << std::endl;
    return true;
  } else {
    std::cout << "Failed to unregister: " << reply.message() << std::endl;
    return false;
  }
}

// Client communication client implementation
ClientCommunicationClient::ClientCommunicationClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(helloworld::ClientCommunication::NewStub(channel)) {}

bool ClientCommunicationClient::SendMessage(const std::string& from_client_id,
                                            const std::string& to_client_id,
                                            const std::string& message_content) const {
  helloworld::ClientMessage request;
  request.set_from_client_id(from_client_id);
  request.set_to_client_id(to_client_id);
  request.set_message_content(message_content);
  
  // Set timestamp
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  request.set_timestamp(std::to_string(time_t));
  
  helloworld::MessageResponse reply;
  grpc::ClientContext context;
  
  grpc::Status status = stub_->SendMessage(&context, request, &reply);
  
  if (status.ok() && reply.success()) {
    std::cout << "Message sent successfully: " << reply.message() << std::endl;
    return true;
  } else {
    std::cout << "Failed to send message: " << reply.message() << std::endl;
    return false;
  }
}

// Main client implementation
Client::Client(const std::string& registry_server_address,
               const std::string& client_id,
               const std::string& client_address,
               int32_t client_port)
    : client_id_(client_id), client_address_(client_address), client_port_(client_port), running_(false) {
  
  // Create registry client
  auto registry_channel = grpc::CreateChannel(registry_server_address, grpc::InsecureChannelCredentials());
  registry_client_ = std::make_unique<ClientRegistryClient>(registry_channel);
  
  // Create communication service
  communication_service_ = std::make_unique<ClientCommunicationServiceImpl>();
}

bool Client::Start() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  
  if (running_) {
    std::cout << "Client is already running" << std::endl;
    return false;
  }
  
  // Register with registry
  if (!registry_client_->RegisterClient(client_id_, client_address_, client_port_)) {
    std::cout << "Failed to register with registry" << std::endl;
    return false;
  }
  
  // Start communication server
  const std::string full_address = client_address_ + ":" + std::to_string(client_port_);
  
  grpc::ServerBuilder builder;
  builder.AddListeningPort(full_address, grpc::InsecureServerCredentials());
  builder.RegisterService(communication_service_.get());
  
  communication_server_ = builder.BuildAndStart();
  
  if (!communication_server_) {
    std::cout << "Failed to start communication server" << std::endl;
    return false;
  }
  
  std::cout << "Client communication server listening on " << full_address << std::endl;
  
  // Start server in a separate thread
  server_thread_ = std::thread([this]() {
    communication_server_->Wait();
  });
  
  // Message polling is not needed since messages are printed immediately
  // when received via gRPC SendMessage call
  
  running_ = true;
  std::cout << "Client started successfully" << std::endl;
  
  return true;
}

bool Client::SendMessageToClient(const std::string& target_client_id, const std::string& message) {
  // Get target client info from registry
  std::string target_address;
  int32_t target_port;
  bool target_online;
  
  if (!registry_client_->GetClient(target_client_id, target_address, target_port, target_online)) {
    std::cout << "Failed to get target client info" << std::endl;
    return false;
  }
  
  if (!target_online) {
    std::cout << "Target client is not online" << std::endl;
    return false;
  }
  
  // Create connection to target client
  std::string target_full_address = target_address + ":" + std::to_string(target_port);
  auto target_channel = grpc::CreateChannel(target_full_address, grpc::InsecureChannelCredentials());
  auto target_client = std::make_unique<ClientCommunicationClient>(target_channel);
  
  // Send message
  return target_client->SendMessage(client_id_, target_client_id, message);
}

std::vector<std::tuple<std::string, std::string, int32_t, bool>> Client::GetAvailableClients() {
  return registry_client_->ListClients();
}

void Client::Stop() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  
  if (!running_) {
    return;
  }
  
  // Unregister from registry
  registry_client_->UnregisterClient(client_id_);
  
  // Stop communication server
  if (communication_server_) {
    communication_server_->Shutdown();
  }
  
  if (server_thread_.joinable()) {
    server_thread_.join();
  }
  
  // No message polling thread to join
  
  running_ = false;
  std::cout << "Client stopped" << std::endl;
}

// PollMessages method removed - messages are printed immediately when received

// GetCommunicationService method removed - not needed

void RunClientCommunicationServer(const std::string& client_address, int32_t client_port) {
  const std::string full_address = client_address + ":" + std::to_string(client_port);
  ClientCommunicationServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::ServerBuilder builder;
  builder.AddListeningPort(full_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Client communication server listening on " << full_address << std::endl;

  server->Wait();
}

}  // namespace helloworld


