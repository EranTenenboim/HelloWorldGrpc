# HelloWorldGrpc

A simple gRPC Hello World example in C++ using the Bazel build system with explicit target definitions and comprehensive testing.

## Features

- **Explicit Build Configuration**: Uses explicit target definitions in BUILD files for better control
- **Modular Structure**: Separate BUILD files for each component (proto, server, client, tests)
- **Modern Bazel Setup**: Uses Bzlmod (MODULE.bazel) for dependency management
- **Comprehensive Testing**: Unit tests, integration tests, and test coverage
- **Clean Project Structure**: Organized with separate directories for protocol definitions, server, client, and tests

## Project Structure

```
HelloWorldGrpc/
├── MODULE.bazel         # Bzlmod module definition
├── proto/               # Protocol buffer definitions
│   ├── BUILD.bazel      # Proto build configuration
│   └── helloworld.proto
├── srv/                 # Server source code
│   ├── BUILD            # Server build configuration
│   ├── main.cc          # Server main entry point
│   ├── server.cc        # Server implementation
│   └── server.h         # Server header
├── cli/                 # Client source code
│   ├── BUILD            # Client build configuration
│   ├── main.cc          # Client main entry point
│   ├── client.cc        # Client implementation
│   └── client.h         # Client header
├── test/                # Test suite
│   ├── BUILD            # Test build configuration
│   ├── integration_test.cc
│   ├── cli/
│   │   ├── BUILD
│   │   └── client_test.cc
│   └── srv/
│       ├── BUILD
│       └── server_test.cc
├── run_tests.sh         # Test runner script
└── README.md
```

## Prerequisites

- **OS**: Ubuntu 22.04+ (Linux-only)
- **Bazel**: 7.0+ (8.x supported)
- **C++ Compiler**: GCC 9.0+ with C++17 support

All gRPC and Protocol Buffer dependencies are managed automatically by Bazel through the `MODULE.bazel` file. No manual installation of system libraries is required.

## Dependencies

### Automatic dependency management

All dependencies are now managed automatically by Bazel through the `MODULE.bazel` file. This ensures:

- ✅ **Reproducible builds** across different environments
- ✅ **No system library conflicts** 
- ✅ **Automatic version resolution**
- ✅ **Cross-platform compatibility**

### Bazel dependencies (auto-managed)

| Module | Version | Description |
|--------|---------|-------------|
| **rules_proto** | 5.3.0-21.7 | Protocol buffer rules for Bazel |
| **rules_cc** | 0.1.4 | C++ rules for Bazel |
| **protobuf** | 32.0 | Protocol Buffers library |
| **grpc** | 1.74.0 | gRPC C++ library |
| **gtest** | 1.14.0 | Google Test framework (via WORKSPACE) |

### Automatic installation (recommended)

```bash
# Run the setup script - it will check and install all dependencies automatically
./setup.sh
```

The setup script will:
- Check prerequisites (Bazel, protoc, pkg-config, C++ compiler)
- Install missing dependencies automatically (Ubuntu 22.04+)
- Provide clear instructions if manual steps are needed

### Manual installation (alternative)

If you prefer manual installation:

```bash
# Install Bazel
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt update && sudo apt install bazel

# Install build tools
sudo apt install -y build-essential

# Install protobuf tools (headers are vendored via Bazel)
sudo apt install -y protobuf-compiler pkg-config

# Install Google Test (optional, for tests)
sudo apt install libgtest-dev

# Install lcov (optional, for coverage)
sudo apt install lcov
```

### Version compatibility

| Bazel | Protobuf | gRPC | C++ Standard |
|-------|----------|------|--------------|
| 7.0+  | 32.0     | 1.74.0| C++17        |
| 8.0+  | 32.0     | 1.74.0| C++17        |

## Build

The build system uses explicit target definitions in BUILD files:

```bash
# Build all targets
bazel build //...

# Build specific targets
bazel build //srv:server
bazel build //cli:client
```

### Generated Files

Bazel will automatically generate the necessary protobuf and gRPC files during build:
- Protocol buffer C++ files (`*.pb.cc`, `*.pb.h`) - Generated in `bazel-bin/proto/`
- gRPC C++ files (`*.grpc.pb.cc`, `*.grpc.pb.h`) - Generated in `bazel-bin/proto/`

**Note**: These generated files are automatically excluded from git via `.gitignore` and should not be committed to version control.

## Run

### Start the Server

```bash
# Run server in foreground
bazel run //srv:server

# Or run the binary directly
./bazel-bin/srv/server
```

### Run the Client

```bash
# Run client (default: connects to localhost:50051)
bazel run //cli:client

# Run client with custom server address
bazel run //cli:client -- localhost:50051

# Run client with custom server and user name
bazel run //cli:client -- localhost:50051 "YourName"

# Or run the binary directly
./bazel-bin/cli/client
```

## Testing

### Run All Tests

```bash
# Run the comprehensive test suite
./run_tests.sh

# Or run tests individually with Bazel
bazel test //test:all_tests --test_output=all
```

### CI/CD pipeline

This project uses GitHub Actions for CI only on pull requests targeting `main` (manual runs supported):

- Linux-only matrix: Ubuntu 22.04 and 24.04
- Security and documentation checks included

#### Pipeline Stages:
1. **Setup & Validate**: Prerequisites and environment validation
2. **Build**: Compiles all targets with Bazel
3. **Test Suite**: Unit tests, integration tests, and coverage
4. **Test matrix**: Ubuntu 22.04/24.04
5. **Security & Quality**: Security scans and code quality
6. **Performance**: Performance benchmarks (main branch only)
7. **Documentation**: Validates README and scripts
8. **Final Validation**: Ensures all checks passed

#### Required Checks for PRs:
- ✅ All builds must pass
- ✅ All tests must pass  
- ✅ Security checks must pass
- ✅ Documentation must be valid
- ✅ Code coverage must not decrease

See [Branch Protection Guide](.github/BRANCH_PROTECTION.md) for detailed configuration.

### Individual Test Suites

```bash
# Run client unit tests
bazel test //test/cli:client_test --test_output=all

# Run server unit tests  
bazel test //test/srv:server_test --test_output=all

# Run integration tests
bazel test //test:integration_test --test_output=all
```

### Test Coverage

```bash
# Generate test coverage report
bazel coverage //test/cli:client_test //test/srv:server_test //test:integration_test --combined_report=lcov

# View coverage report (requires lcov tools)
genhtml bazel-testlogs/coverage/combined/lcov/coverage.dat --output-directory coverage_html
```

## Development

### Quick Start

```bash
# Clone the repository
git clone <repository-url>
cd HelloWorldGrpc

# Run setup script (automatically installs all dependencies)
./setup.sh

# Build the project
bazel build //...

# Run tests
./run_tests.sh

# Start server (in one terminal)
bazel run //srv:server

# Run client (in another terminal)
bazel run //cli:client
```

### Alternative: CMake build

Removed. The project is Bazel-only.

### Contributing

1. **Fork the repository** and create a feature branch
2. **Run setup**: `./setup.sh` to ensure all prerequisites
3. **Make changes** and test locally: `./run_tests.sh`
4. **Create Pull Request** - CI will automatically test your changes
5. **Wait for review** and approval from maintainers

### Adding New Files

To add new files, you need to update the corresponding BUILD files:

- **New Protocol Files**: Add `.proto` files to `proto/` directory and update `proto/BUILD.bazel`
- **New Server Files**: Add `.cc` files to `srv/` directory and update `srv/BUILD`
- **New Client Files**: Add `.cc` files to `cli/` directory and update `cli/BUILD`
- **New Test Files**: Add test files to `test/` directory and update the corresponding `test/*/BUILD` files

The build system uses explicit target definitions for better control and clarity.

### Test Structure

The project includes comprehensive testing:

- **Unit Tests**: Individual component tests in `test/cli/` and `test/srv/`
- **Integration Tests**: End-to-end tests in `test/integration_test.cc`
- **Test Runner**: Automated test execution via `run_tests.sh`
- **Coverage**: Test coverage reporting with lcov integration

### Clean Build

```bash
# Clean all build artifacts
bazel clean

# Clean and rebuild
bazel clean && bazel build //...
```

## Troubleshooting

### Common Build Issues

#### 1. Bazel Not Found
```bash
# Error: bazel: command not found
# Solution: Install Bazel
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt update && sudo apt install bazel
```

#### 2. Build Failures
```bash
# Clean build cache and rebuild
bazel clean
bazel build //...

# If still failing, check Bazel version
bazel version
# Should be 6.0+ for this project
```

#### 3. Missing C++ Compiler
```bash
# Error: No C++ compiler found
# Solution: Install build tools
sudo apt update
sudo apt install build-essential
```

#### 4. Protocol Buffer Generation Issues
```bash
# If protobuf files are missing or outdated
bazel clean
bazel build //proto:helloworld_cc_proto
bazel build //proto:helloworld_grpc_cc_proto
```

### Runtime Issues

#### 1. Server Won't Start
```bash
# Check if port 50051 is already in use
lsof -i :50051
# Kill any existing process on that port
sudo kill -9 <PID>

# Run server with verbose output
bazel run //srv:server -- --verbose
```

#### 2. Client Connection Failed
```bash
# Error: Failed to connect to localhost:50051
# Solution: Make sure server is running first
# Terminal 1: Start server
bazel run //srv:server

# Terminal 2: Run client
bazel run //cli:client
```

#### 3. Client with Custom Server Address
```bash
# Connect to different server address
bazel run //cli:client -- 192.168.1.100:50051

# Connect with custom user name
bazel run //cli:client -- localhost:50051 "YourName"
```

### Test Issues

#### 1. Tests Failing
```bash
# Run tests with detailed output
bazel test //test:all_tests --test_output=all --verbose_failures

# Run individual test suites
bazel test //test/cli:client_test --test_output=all
bazel test //test/srv:server_test --test_output=all
bazel test //test:integration_test --test_output=all
```

#### 2. Test Coverage Issues
```bash
# Install lcov for coverage reports
sudo apt install lcov

# Generate coverage report
bazel coverage //test/cli:client_test //test/srv:server_test //test:integration_test --combined_report=lcov

# View coverage in browser
genhtml bazel-testlogs/coverage/combined/lcov/coverage.dat --output-directory coverage_html
open coverage_html/index.html
```

#### 3. Integration Test Port Conflicts
```bash
# If integration tests fail due to port conflicts
# The tests use dynamic port allocation (localhost:0)
# If still failing, check for firewall issues
sudo ufw status
# Allow local connections if needed
```

### Development Issues

#### 1. Adding New Dependencies
```bash
# For new Bazel dependencies, update MODULE.bazel
# For new C++ dependencies, update BUILD files

# Example: Adding a new library to server
# In srv/BUILD, add to deps:
# deps = [
#     "//proto:helloworld_cc_proto",
#     "//proto:helloworld_grpc_cc_proto", 
#     "@grpc//:grpc++",
#     "@new_library//:target",  # Add new dependency
# ],
```

#### 2. Build File Syntax Errors
```bash
# Check BUILD file syntax
bazel query //srv:server  # Should not error
bazel query //cli:client  # Should not error
bazel query //test:all_tests  # Should not error
```

#### 3. Generated Files Issues
```bash
# If protobuf/gRPC generated files are missing
# They should be generated automatically, but if not:
bazel build //proto:helloworld_cc_proto
bazel build //proto:helloworld_grpc_cc_proto

# Check if files exist in bazel-bin
ls -la bazel-bin/proto/
```

### Performance Issues

#### 1. Slow Builds
```bash
# Use Bazel's remote caching (if available)
bazel build //... --remote_cache=grpc://your-cache-server:8080

# Or use local disk cache
bazel build //... --disk_cache=/tmp/bazel-cache
```

#### 2. Memory Issues
```bash
# Increase Bazel heap size
export BAZEL_JAVAC_OPTS="-J-Xmx2g"
bazel build //...
```

### Debugging

#### 1. Verbose Build Output
```bash
# Get detailed build information
bazel build //... --verbose_failures --verbose_explanations

# Check dependency graph
bazel query --output=graph //srv:server | dot -Tpng > deps.png
```

#### 2. Debugging Runtime Issues
```bash
# Run with debug symbols
bazel build //srv:server --compilation_mode=dbg
bazel build //cli:client --compilation_mode=dbg

# Use gdb for debugging
gdb bazel-bin/srv/server
gdb bazel-bin/cli/client
```

### System Requirements

#### Minimum requirements
- **OS**: Ubuntu 22.04+
- **RAM**: 4GB minimum, 8GB recommended
- **Disk**: 2GB free space for dependencies
- **Network**: Internet connection for dependency downloads

#### Recommended development environment
- **OS**: Ubuntu 22.04/24.04
- **RAM**: 16GB
- **CPU**: 4+ cores
- **Disk**: 10GB+ free space
- **IDE**: VS Code with C++ extension, CLion, or similar

### Getting Help

If you encounter issues not covered here:

1. **Check Bazel logs**: `bazel build //... --verbose_failures`
2. **Verify dependencies**: `bazel query //...`
3. **Clean and rebuild**: `bazel clean && bazel build //...`
4. **Check system requirements**: Ensure all prerequisites are installed
5. **Review BUILD files**: Verify target definitions are correct

### Build Configuration Reference

The project uses separate BUILD files for each component:
- `proto/BUILD.bazel` - Defines protocol buffer and gRPC targets
- `srv/BUILD` - Defines server binary target with greeter_service library
- `cli/BUILD` - Defines client binary target with greeter_client library  
- `test/BUILD` - Defines integration test target
- `test/cli/BUILD` - Defines client unit tests
- `test/srv/BUILD` - Defines server unit tests

Each BUILD file explicitly lists source files and dependencies for better build control and clarity.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
