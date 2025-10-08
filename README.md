# HelloWorldGrpc

A simple gRPC Hello World example in C++ using Bazel build system with generalized file discovery.

## Features

- **Generalized Build Configuration**: Uses Bazel `glob()` patterns to automatically discover source files
- **Automatic File Discovery**: Add new `.proto`, `.cc` files and they're automatically included in the build
- **Modern Bazel Setup**: Uses Bzlmod (MODULE.bazel) for dependency management
- **Clean Project Structure**: Organized with separate directories for protocol definitions, server, and client code

## Project Structure

```
HelloWorldGrpc/
├── BUILD                 # Bazel build configuration with glob patterns
├── MODULE.bazel         # Bzlmod module definition
├── proto/               # Protocol buffer definitions
│   └── helloworld.proto
├── srv/                 # Server source code
│   └── server.cc
├── cli/                 # Client source code
│   └── client.cc
└── README.md
```

## Prerequisites

- **Bazel**: Install Bazel (version 6.0+ recommended)
- **Protocol Buffers**: `protoc` compiler and development libraries
- **gRPC**: gRPC C++ development libraries and plugins

### Ubuntu/Debian Installation

```bash
# Install Bazel
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt update && sudo apt install bazel

# Install protobuf and gRPC
sudo apt install protobuf-compiler libprotobuf-dev libgrpc++-dev protobuf-compiler-grpc
```

## Build

The build system automatically discovers files using glob patterns:

```bash
# Build all targets
bazel build //...

# Build specific targets
bazel build //:hello_server
bazel build //:hello_client
```

### Generated Files

Bazel will automatically generate the necessary protobuf and gRPC files:
- Protocol buffer C++ files (`*.pb.cc`, `*.pb.h`)
- gRPC C++ files (`*.grpc.pb.cc`, `*.grpc.pb.h`)

## Run

### Start the Server

```bash
# Run server in foreground
bazel run //:hello_server

# Or run the binary directly
./bazel-bin/hello_server
```

### Run the Client

```bash
# Run client
bazel run //:hello_client

# Or run the binary directly
./bazel-bin/hello_client
```

## Development

### Adding New Files

The build system uses glob patterns, so you can easily add new files:

- **New Protocol Files**: Add `.proto` files to `proto/` directory
- **New Server Files**: Add `.cc` files to `srv/` directory  
- **New Client Files**: Add `.cc` files to `cli/` directory

The build system will automatically discover and include them.

### Clean Build

```bash
# Clean all build artifacts
bazel clean

# Clean and rebuild
bazel clean && bazel build //...
```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**: Ensure all prerequisites are installed
2. **Version Compatibility**: Make sure protobuf and gRPC versions are compatible
3. **Build Cache**: Use `bazel clean` if you encounter build issues

### Build Configuration

The `BUILD` file uses the following glob patterns:
- `glob(["proto/*.proto"])` - Discovers all protocol buffer files
- `glob(["srv/*.cc"])` - Discovers all server source files
- `glob(["cli/*.cc"])` - Discovers all client source files
- `glob(["proto/*.grpc.pb.cc"], allow_empty = True)` - Discovers generated gRPC files

## License

This project is licensed under the MIT License - see the LICENSE file for details.
