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
- **C++ Compiler**: GCC 9.0+ or Clang with C++17 support

**Note**: All gRPC and Protocol Buffer dependencies are now managed automatically by Bazel through the `MODULE.bazel` file. No manual installation of system libraries is required!

## Dependencies

### Automatic Dependency Management

All dependencies are now managed automatically by Bazel through the `MODULE.bazel` file. This ensures:

- ✅ **Reproducible builds** across different environments
- ✅ **No system library conflicts** 
- ✅ **Automatic version resolution**
- ✅ **Cross-platform compatibility**

### Bazel Dependencies (Auto-managed)

| Module | Version | Description |
|--------|---------|-------------|
| **rules_proto** | 1.0.0 | Protocol buffer rules for Bazel |
| **rules_cc** | 0.0.9 | C++ rules for Bazel |
| **bazel_skylib** | 1.4.2 | Skylib utilities for Bazel |
| **com_google_protobuf** | 21.12 | Protocol Buffers library |
| **rules_proto_grpc** | 4.1.1 | gRPC protocol buffer rules |
| **com_github_grpc_grpc** | 1.50.1 | gRPC C++ library |
| **upb** | 0.0.0-20221230-1.1.0 | Micro protobuf library |
| **re2** | 2022-12-01 | Regular expression library |
| **abseil-cpp** | 20220623.1 | Google's C++ common libraries |
| **zlib** | 1.2.13 | Compression library |
| **c-ares** | 1.19.1 | Asynchronous DNS resolution library |

### Simple Installation

```bash
# Install Bazel only
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt update && sudo apt install bazel

# Install C++ compiler
sudo apt install build-essential
```

### Version Compatibility

| Bazel | Protobuf | gRPC | C++ Standard |
|-------|----------|------|--------------|
| 6.0+  | 21.12    | 1.50.1| C++17        |
| 7.0+  | 21.12    | 1.50.1| C++17        |

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
