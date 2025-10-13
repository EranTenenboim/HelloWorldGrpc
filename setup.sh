#!/bin/bash

# HelloWorldGrpc Setup Script
# This script checks prerequisites, verifies versions, and installs missing dependencies
# Following Google standards for automated setup

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to get version number
get_version() {
    local cmd="$1"
    local version_flag="$2"
    if command_exists "$cmd"; then
        $cmd $version_flag 2>/dev/null | head -n1 | grep -oE '[0-9]+\.[0-9]+(\.[0-9]+)?' | head -n1
    else
        echo "0.0.0"
    fi
}

# Function to compare version numbers
# Returns: 0 if equal, 1 if version1 > version2, 2 if version1 < version2
version_compare() {
    local version1="$1"
    local version2="$2"
    if [[ "$version1" == "$version2" ]]; then
        return 0
    fi
    local IFS=.
    local i ver1=($version1) ver2=($version2)
    # Pad shorter version with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++)); do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++)); do
        if [[ -z ${ver2[i]} ]]; then
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]})); then
            return 1  # version1 > version2
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]})); then
            return 2  # version1 < version2
        fi
    done
    return 0  # equal
}

# Function to check if version meets minimum requirement
version_meets_requirement() {
    local current_version="$1"
    local required_version="$2"
    version_compare "$current_version" "$required_version"
    local result=$?
    # Return 0 (success) if current >= required
    # Return 1 (failure) if current < required
    if [[ $result -eq 0 || $result -eq 1 ]]; then
        return 0  # current >= required
    else
        return 1  # current < required
    fi
}

# Function to install Bazel
install_bazel() {
    print_info "Installing Bazel..."
    
    # Detect OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Ubuntu/Debian
        if command_exists apt-get; then
            print_info "Installing Bazel on Ubuntu/Debian..."
            curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg
            sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
            echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
            sudo apt update
            sudo apt install -y bazel
        else
            print_error "apt-get not found. Please install Bazel manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command_exists brew; then
            print_info "Installing Bazel on macOS via Homebrew..."
            brew install bazel
        else
            print_error "Homebrew not found. Please install Bazel manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install Bazel manually."
        return 1
    fi
}

# Function to install CMake
install_cmake() {
    print_info "Installing CMake..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing CMake on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y cmake
        else
            print_error "apt-get not found. Please install CMake manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            print_info "Installing CMake on macOS via Homebrew..."
            brew install cmake
        else
            print_error "Homebrew not found. Please install CMake manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install CMake manually."
        return 1
    fi
}

# Function to install protoc
install_protoc() {
    print_info "Installing protoc..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing protoc on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y protobuf-compiler libprotobuf-dev
        else
            print_error "apt-get not found. Please install protoc manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            print_info "Installing protoc on macOS via Homebrew..."
            brew install protobuf
        else
            print_error "Homebrew not found. Please install protoc manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install protoc manually."
        return 1
    fi
}

# Function to install gRPC dependencies
install_grpc() {
    print_info "Installing gRPC dependencies..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing gRPC on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y libgrpc++-dev libgrpc-dev
            # Try to install gRPC tools that include the plugin
            sudo apt install -y grpc-dev-tools || sudo apt install -y grpc-tools || print_warning "gRPC tools not available, continuing..."
        else
            print_error "apt-get not found. Please install gRPC manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            print_info "Installing gRPC on macOS via Homebrew..."
            brew install grpc
        else
            print_error "Homebrew not found. Please install gRPC manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install gRPC manually."
        return 1
    fi
}

# Function to install pkg-config
install_pkg_config() {
    print_info "Installing pkg-config..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing pkg-config on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y pkg-config
        else
            print_error "apt-get not found. Please install pkg-config manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            print_info "Installing pkg-config on macOS via Homebrew..."
            brew install pkg-config
        else
            print_error "Homebrew not found. Please install pkg-config manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install pkg-config manually."
        return 1
    fi
}

# Function to install Google Test
install_gtest() {
    print_info "Installing Google Test..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing Google Test on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y libgtest-dev
        else
            print_error "apt-get not found. Please install Google Test manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            print_info "Installing Google Test on macOS via Homebrew..."
            brew install googletest
        else
            print_error "Homebrew not found. Please install Google Test manually or install Homebrew first."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install Google Test manually."
        return 1
    fi
}

# Function to install C++ compiler
install_cpp_compiler() {
    print_info "Installing C++ compiler..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            print_info "Installing build-essential on Ubuntu/Debian..."
            sudo apt update
            sudo apt install -y build-essential
        else
            print_error "apt-get not found. Please install C++ compiler manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists xcode-select; then
            print_info "Installing Xcode command line tools on macOS..."
            xcode-select --install
        else
            print_error "xcode-select not found. Please install Xcode command line tools manually."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE. Please install C++ compiler manually."
        return 1
    fi
}

# Function to install lcov (for coverage reports)
install_lcov() {
    print_info "Installing lcov for coverage reports..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command_exists apt-get; then
            sudo apt update
            sudo apt install -y lcov
        else
            print_warning "apt-get not found. Please install lcov manually for coverage reports."
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command_exists brew; then
            brew install lcov
        else
            print_warning "Homebrew not found. Please install lcov manually for coverage reports."
        fi
    else
        print_warning "Unsupported OS: $OSTYPE. Please install lcov manually for coverage reports."
    fi
}

# Function to check system requirements
check_system_requirements() {
    print_info "Checking system requirements..."
    
    # Check available memory (basic check)
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        local mem_gb=$(free -g | awk '/^Mem:/{print $2}')
        if [[ $mem_gb -lt 4 ]]; then
            print_warning "System has ${mem_gb}GB RAM. Recommended: 8GB+ for optimal performance."
        else
            print_success "System memory: ${mem_gb}GB (OK)"
        fi
    fi
    
    # Check disk space
    local disk_space=$(df -BG . | awk 'NR==2 {print $4}' | sed 's/G//')
    if [[ $disk_space -lt 2 ]]; then
        print_warning "Low disk space: ${disk_space}GB available. Recommended: 10GB+ free space."
    else
        print_success "Disk space: ${disk_space}GB available (OK)"
    fi
}

# Main setup function
main() {
    echo "=========================================="
    echo "HelloWorldGrpc Setup Script"
    echo "=========================================="
    echo ""
    
    # Check system requirements
    check_system_requirements
    echo ""
    
    # Variables for requirements
    local bazel_required="6.0.0"
    local cmake_required="3.16.0"
    local gcc_required="9.0.0"
    local clang_required="9.0.0"
    local protoc_required="3.0.0"
    local missing_deps=()
    local outdated_deps=()
    
    print_info "Checking prerequisites..."
    echo ""
    
    # Check Bazel
    print_info "Checking Bazel..."
    if command_exists bazel; then
        local bazel_version=$(get_version "bazel" "version")
        if version_meets_requirement "$bazel_version" "$bazel_required"; then
            print_success "Bazel $bazel_version (meets requirement: $bazel_required+)"
        else
            print_warning "Bazel $bazel_version (required: $bazel_required+)"
            outdated_deps+=("bazel")
        fi
    else
        print_error "Bazel not found (required: $bazel_required+)"
        missing_deps+=("bazel")
    fi
    
    # Check CMake
    print_info "Checking CMake..."
    if command_exists cmake; then
        local cmake_version=$(get_version "cmake" "--version")
        if version_meets_requirement "$cmake_version" "$cmake_required"; then
            print_success "CMake $cmake_version (meets requirement: $cmake_required+)"
        else
            print_warning "CMake $cmake_version (required: $cmake_required+)"
            outdated_deps+=("cmake")
        fi
    else
        print_error "CMake not found (required: $cmake_required+)"
        missing_deps+=("cmake")
    fi
    
    # Check protoc
    print_info "Checking protoc..."
    if command_exists protoc; then
        local protoc_version=$(get_version "protoc" "--version")
        if version_meets_requirement "$protoc_version" "$protoc_required"; then
            print_success "protoc $protoc_version (meets requirement: $protoc_required+)"
        else
            print_warning "protoc $protoc_version (required: $protoc_required+)"
            outdated_deps+=("protoc")
        fi
    else
        print_error "protoc not found (required: $protoc_required+)"
        missing_deps+=("protoc")
    fi
    
    # Check pkg-config first
    print_info "Checking pkg-config..."
    if command_exists pkg-config; then
        print_success "pkg-config found"
        
        # Check gRPC (for CMake builds)
        print_info "Checking gRPC..."
        if pkg-config --exists grpc++; then
            local grpc_version=$(pkg-config --modversion grpc++)
            print_success "gRPC $grpc_version"
        else
            print_warning "gRPC not found (required for CMake builds)"
            missing_deps+=("grpc")
        fi
        
        # Check Google Test (optional, for tests)
        print_info "Checking Google Test (optional, for tests)..."
        if pkg-config --exists gtest; then
            local gtest_version=$(pkg-config --modversion gtest)
            print_success "Google Test $gtest_version"
        else
            print_warning "Google Test not found (optional, for tests)"
            missing_deps+=("gtest")
        fi
    else
        print_warning "pkg-config not found (required for dependency checking)"
        missing_deps+=("pkg-config")
    fi
    
    # Check C++ compiler
    print_info "Checking C++ compiler..."
    local cpp_compiler_found=false
    
    if command_exists gcc; then
        local gcc_version=$(get_version "gcc" "--version")
        if version_meets_requirement "$gcc_version" "$gcc_required"; then
            print_success "GCC $gcc_version (meets requirement: $gcc_required+)"
            cpp_compiler_found=true
        else
            print_warning "GCC $gcc_version (required: $gcc_required+)"
            outdated_deps+=("gcc")
        fi
    fi
    
    if command_exists clang; then
        local clang_version=$(get_version "clang" "--version")
        if version_meets_requirement "$clang_version" "$clang_required"; then
            print_success "Clang $clang_version (meets requirement: $clang_required+)"
            cpp_compiler_found=true
        else
            print_warning "Clang $clang_version (required: $clang_required+)"
            outdated_deps+=("clang")
        fi
    fi
    
    if [[ "$cpp_compiler_found" == false ]]; then
        print_error "No suitable C++ compiler found (required: GCC $gcc_required+ or Clang $clang_required+)"
        missing_deps+=("cpp_compiler")
    fi
    
    # Check lcov (optional)
    print_info "Checking lcov (optional, for coverage reports)..."
    if command_exists lcov; then
        local lcov_version=$(get_version "lcov" "--version")
        print_success "lcov $lcov_version"
    else
        print_warning "lcov not found (optional, for coverage reports)"
        # Don't add to missing_deps since it's optional
    fi
    
    echo ""
    
    # Summary
    if [[ ${#missing_deps[@]} -eq 0 && ${#outdated_deps[@]} -eq 0 ]]; then
        print_success "All prerequisites are satisfied!"
        echo ""
        print_info "You can now build and run the project:"
        echo "  bazel build //..."
        echo "  bazel run //srv:server"
        echo "  bazel run //cli:client"
        echo "  ./run_tests.sh"
        echo ""
        print_info "Optional: Install lcov for coverage reports:"
        echo "  sudo apt install lcov  # Ubuntu/Debian"
        echo "  brew install lcov     # macOS"
        return 0
    fi
    
    # Handle missing or outdated dependencies
    echo "=========================================="
    print_info "Dependency Summary:"
    echo "=========================================="
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_error "Missing dependencies:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
    fi
    
    if [[ ${#outdated_deps[@]} -gt 0 ]]; then
        print_warning "Outdated dependencies:"
        for dep in "${outdated_deps[@]}"; do
            echo "  - $dep"
        done
    fi
    
    echo ""
    
    # Ask for installation
    if [[ ${#missing_deps[@]} -gt 0 || ${#outdated_deps[@]} -gt 0 ]]; then
        read -p "Would you like to install/update missing dependencies? (y/N): " -n 1 -r
        echo ""
        
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            echo ""
            print_info "Installing/updating dependencies..."
            
            # Install missing dependencies
            for dep in "${missing_deps[@]}"; do
                case $dep in
                    "bazel")
                        install_bazel
                        ;;
                    "cmake")
                        install_cmake
                        ;;
                    "protoc")
                        install_protoc
                        ;;
                    "pkg-config")
                        install_pkg_config
                        ;;
                    "grpc")
                        install_grpc
                        ;;
                    "gtest")
                        install_gtest
                        ;;
                    "cpp_compiler")
                        install_cpp_compiler
                        ;;
                    "lcov")
                        install_lcov
                        ;;
                esac
            done
            
            # Handle outdated dependencies
            for dep in "${outdated_deps[@]}"; do
                case $dep in
                    "bazel")
                        print_info "Updating Bazel..."
                        install_bazel
                        ;;
                    "cmake")
                        print_info "Updating CMake..."
                        install_cmake
                        ;;
                    "protoc")
                        print_info "Updating protoc..."
                        install_protoc
                        ;;
                    "grpc")
                        print_info "Updating gRPC..."
                        install_grpc
                        ;;
                    "gtest")
                        print_info "Updating Google Test..."
                        install_gtest
                        ;;
                    "gcc"|"clang")
                        print_info "Updating C++ compiler..."
                        install_cpp_compiler
                        ;;
                esac
            done
            
            echo ""
            print_success "Installation completed!"
            echo ""
            print_info "Please run this script again to verify all dependencies:"
            echo "  ./setup.sh"
        else
            echo ""
            print_info "Manual installation required. Please install the missing dependencies:"
            echo ""
            if [[ " ${missing_deps[@]} " =~ " bazel " ]]; then
                echo "Bazel: https://bazel.build/install"
            fi
            if [[ " ${missing_deps[@]} " =~ " cpp_compiler " ]]; then
                echo "C++ Compiler: Install build-essential (Ubuntu) or Xcode tools (macOS)"
            fi
            if [[ " ${missing_deps[@]} " =~ " lcov " ]]; then
                echo "lcov: sudo apt install lcov (Ubuntu) or brew install lcov (macOS)"
            fi
        fi
    fi
}

# Run main function
main "$@"
