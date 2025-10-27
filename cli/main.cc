#include "client.h"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

void print_usage(const char* program_name) {
  std::cout << "Usage: " << program_name << " [options]\n";
  std::cout << "Options:\n";
  std::cout << "  -s <server_address>    Registry server address (default: localhost:50051)\n";
  std::cout << "  -i <client_id>          Client ID (required)\n";
  std::cout << "  -a <client_address>    Client listening address (default: localhost)\n";
  std::cout << "  -p <client_port>        Client listening port (default: 50052)\n";
  std::cout << "  -u <target_client_id>   Target client ID for message\n";
  std::cout << "  -m <message>            Message to send to target client\n";
  std::cout << "  -l                     List available clients\n";
  std::cout << "  -h                     Show this help message\n";
}

int main(const int argc, const char* const argv[]) {
  std::string registry_server_address = "localhost:50051";
  std::string client_address = "localhost";
  int32_t client_port = 50052;
  std::string client_id = "";
  std::string target_client_id = "";
  std::string message = "";
  bool list_clients = false;
  
  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    
    if (arg == "-s" && i + 1 < argc) {
      registry_server_address = argv[++i];
    } else if (arg == "-i" && i + 1 < argc) {
      client_id = argv[++i];
    } else if (arg == "-a" && i + 1 < argc) {
      client_address = argv[++i];
    } else if (arg == "-p" && i + 1 < argc) {
      client_port = std::stoi(argv[++i]);
    } else if (arg == "-u" && i + 1 < argc) {
      target_client_id = argv[++i];
    } else if (arg == "-m" && i + 1 < argc) {
      message = argv[++i];
    } else if (arg == "-l") {
      list_clients = true;
    } else if (arg == "-h") {
      print_usage(argv[0]);
      return 0;
    } else {
      std::cout << "Unknown option: " << arg << std::endl;
      print_usage(argv[0]);
      return 1;
    }
  }
  
  // Validate required arguments
  if (client_id.empty()) {
    std::cout << "Error: Client ID is required (-i)" << std::endl;
    print_usage(argv[0]);
    return 1;
  }
  
  std::cout << "Starting Client Registry System..." << std::endl;
  std::cout << "Registry Server: " << registry_server_address << std::endl;
  std::cout << "Client ID: " << client_id << std::endl;
  std::cout << "Client Address: " << client_address << ":" << client_port << std::endl;
  
  // Create and start client
  helloworld::Client client(registry_server_address, client_id, client_address, client_port);
  
  if (!client.Start()) {
    std::cout << "Failed to start client!" << std::endl;
    return 1;
  }
  
  // Handle different operations
  if (list_clients) {
    std::cout << "\nAvailable clients:" << std::endl;
    auto clients = client.GetAvailableClients();
    for (const auto& [id, address, port, online] : clients) {
      std::cout << "  " << id << " at " << address << ":" << port 
                << " (online: " << (online ? "yes" : "no") << ")" << std::endl;
    }
  }
  
  if (!target_client_id.empty() && !message.empty()) {
    std::cout << "\nSending message to " << target_client_id << ": " << message << std::endl;
    if (client.SendMessageToClient(target_client_id, message)) {
      std::cout << "Message sent successfully!" << std::endl;
    } else {
      std::cout << "Failed to send message!" << std::endl;
    }
  }
  
  if (!list_clients && target_client_id.empty()) {
    std::cout << "\nClient is running and listening for messages..." << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  send <destination> <message>  - Send message to another client" << std::endl;
    std::cout << "  list                         - List available clients" << std::endl;
    std::cout << "  help                         - Show this help" << std::endl;
    std::cout << "  quit                         - Exit client" << std::endl;
    std::cout << "Type commands and press Enter:" << std::endl;
    
    // Interactive command loop
    std::string input;
    while (true) {
      
      std::getline(std::cin, input);
      
      if (input.empty()) {
        continue;
      }
      
      // Parse command
      std::istringstream iss(input);
      std::string command;
      iss >> command;
      
      if (command == "send") {
        std::string destination, message;
        iss >> destination;
        
        // Get the rest of the line as message
        std::string remaining;
        std::getline(iss, remaining);
        if (!remaining.empty() && remaining[0] == ' ') {
          message = remaining.substr(1); // Remove leading space
        }
        
        if (destination.empty() || message.empty()) {
          std::cout << "Usage: send <destination> <message>" << std::endl;
          continue;
        }
        
        std::cout << "Sending message to " << destination << ": " << message << std::endl;
        if (client.SendMessageToClient(destination, message)) {
          std::cout << "Message sent successfully!" << std::endl;
        } else {
          std::cout << "Failed to send message!" << std::endl;
        }
        
      } else if (command == "list") {
        std::cout << "\nAvailable clients:" << std::endl;
        auto clients = client.GetAvailableClients();
        if (clients.empty()) {
          std::cout << "  No clients available" << std::endl;
        } else {
          for (const auto& [id, address, port, online] : clients) {
            std::cout << "  " << id << " at " << address << ":" << port 
                      << " (online: " << (online ? "yes" : "no") << ")" << std::endl;
          }
        }
        
      } else if (command == "help") {
        std::cout << "\nAvailable commands:" << std::endl;
        std::cout << "  send <destination> <message>  - Send message to another client" << std::endl;
        std::cout << "  list                         - List available clients" << std::endl;
        std::cout << "  help                         - Show this help" << std::endl;
        std::cout << "  quit                         - Exit client" << std::endl;
        
      } else if (command == "quit" || command == "exit") {
        std::cout << "Exiting..." << std::endl;
        break;
        
      } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
      }
    }
  }
  
  // Cleanup
  client.Stop();
  
  return 0;
}
