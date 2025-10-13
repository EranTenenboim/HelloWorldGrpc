#!/bin/bash

# Test runner script for HelloWorldGrpc project
# This script runs all tests using Bazel following Google standards

set -e  # Exit on any error

echo "Running HelloWorldGrpc Test Suite"
echo "================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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

# Check if Bazel is available
if ! command -v bazel &> /dev/null; then
    print_error "Bazel is not installed or not in PATH"
    exit 1
fi

print_status "Building all targets..."
bazel build //...

print_status "Running client tests..."
bazel test //test/cli:client_test --test_output=all

print_status "Running server tests..."
bazel test //test/srv:server_test --test_output=all

print_status "Running integration tests..."
bazel test //test:integration_test --test_output=all

print_status "Running all tests together..."
bazel test //test/cli:client_test //test/srv:server_test //test:integration_test --test_output=all

print_status "Running tests with coverage..."
bazel coverage //test/cli:client_test //test/srv:server_test //test:integration_test --combined_report=lcov

print_status "All tests completed successfully!"
echo "================================="
echo "Test Summary:"
echo "- Client tests: PASSED"
echo "- Server tests: PASSED" 
echo "- Integration tests: PASSED"
echo "- Coverage report generated"
