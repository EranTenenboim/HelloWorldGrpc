#!/bin/bash

# CMake Test runner script for HelloWorldGrpc project
# This script builds and tests the project using CMake

set -e  # Exit on any error

echo "Running HelloWorldGrpc CMake Test Suite"
echo "======================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed or not in PATH"
    exit 1
fi

# Check if protoc is available
if ! command -v protoc &> /dev/null; then
    print_error "protoc is not installed or not in PATH"
    exit 1
fi

# Check if gRPC is available
if ! pkg-config --exists grpc++; then
    print_error "gRPC is not installed or not in PATH"
    exit 1
fi

print_status "CMake version: $(cmake --version | head -n1)"
print_status "Protobuf version: $(protoc --version)"
print_status "gRPC version: $(pkg-config --modversion grpc++)"

# Clean previous build
print_info "Cleaning previous build..."
rm -rf build_cmake
mkdir -p build_cmake
cd build_cmake

# Configure CMake
print_info "Configuring CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build the project
print_info "Building project..."
cmake --build . --config Release

# Check if Google Test is available
if cmake --build . --target client_test &> /dev/null; then
    print_info "Google Test found - running tests..."
    
    # Run individual tests
    print_info "Running client tests..."
    ./client_test --gtest_output=xml:client_test_results.xml
    
    print_info "Running server tests..."
    ./server_test --gtest_output=xml:server_test_results.xml
    
    print_info "Running integration tests..."
    ./integration_test --gtest_output=xml:integration_test_results.xml
    
    # Run all tests with CTest
    print_info "Running CTest..."
    ctest --output-on-failure --verbose
    
    print_success "All CMake tests completed successfully!"
    
    # Generate test coverage if lcov is available
    if command -v lcov &> /dev/null; then
        print_info "Generating test coverage..."
        # Note: Coverage generation with CMake requires additional setup
        print_warning "Coverage generation requires additional CMake configuration"
    fi
    
else
    print_warning "Google Test not found - skipping tests"
    print_info "To enable tests, install Google Test:"
    print_info "  Ubuntu: sudo apt install libgtest-dev"
    print_info "  macOS: brew install googletest"
fi

# Test the executables
print_info "Testing server and client executables..."

# Start server in background
print_info "Starting server..."
./server &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Test client
print_info "Testing client..."
if ./client; then
    print_success "Client test passed!"
else
    print_error "Client test failed!"
    kill $SERVER_PID 2>/dev/null || true
    exit 1
fi

# Cleanup
print_info "Stopping server..."
kill $SERVER_PID 2>/dev/null || true

print_success "CMake build and test completed successfully!"
echo "======================================"
echo "Test Summary:"
echo "- CMake build: PASSED"
echo "- Executables: PASSED"
if cmake --build . --target client_test &> /dev/null; then
    echo "- Unit tests: PASSED"
    echo "- Integration tests: PASSED"
fi
echo "- Server/Client communication: PASSED"
