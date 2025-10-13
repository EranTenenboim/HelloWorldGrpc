cmake_minimum_required(VERSION 3.16)
project(HelloWorldGrpc VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

# Find gRPC using pkg-config as primary method
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)

# Don't use find_package(gRPC) as it requires the plugin
# Use pkg-config results instead
set(gRPC_FOUND TRUE)
set(gRPC_LIBRARIES ${GRPC_LIBRARIES})
set(gRPC_INCLUDE_DIRS ${GRPC_INCLUDE_DIRS})

# Check if grpc_cpp_plugin exists, but don't fail if it doesn't
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
if(GRPC_CPP_PLUGIN)
    message(STATUS "Found grpc_cpp_plugin: ${GRPC_CPP_PLUGIN}")
else()
    message(WARNING "grpc_cpp_plugin not found - gRPC generation will be skipped")
endif()


# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/proto)

# Generate protobuf and gRPC files
set(PROTO_FILES proto/helloworld.proto)
set(PROTO_SRCS)
set(PROTO_HDRS)
set(GRPC_SRCS)
set(GRPC_HDRS)

foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    get_filename_component(PROTO_PATH ${PROTO_FILE} PATH)
    
    list(APPEND PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc)
    list(APPEND PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.h)
    list(APPEND GRPC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc)
    list(APPEND GRPC_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.h)
endforeach()

# Generate protobuf files
add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
    COMMAND protoc
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
         ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
    COMMENT "Generating protobuf files"
)

# Generate gRPC files (with fallback for missing plugin)
if(GRPC_CPP_PLUGIN)
    add_custom_command(
        OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
        COMMAND protoc
        ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
             --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
             --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
             ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
        COMMENT "Generating gRPC files"
    )
else()
    message(WARNING "grpc_cpp_plugin not found, creating stub gRPC files")
    # Create stub gRPC files with proper content to avoid build errors
    add_custom_command(
        OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
        COMMAND ${CMAKE_COMMAND} -E echo "// gRPC files not generated - plugin not available" > ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc
        COMMAND ${CMAKE_COMMAND} -E echo "#ifndef HELLOWORLD_GRPC_PB_H" > ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "#define HELLOWORLD_GRPC_PB_H" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "#include \"helloworld.pb.h\"" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "#include <grpcpp/grpcpp.h>" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "namespace helloworld {" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "class Greeter {" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "public:" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  class Service {" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  public:" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "    virtual ~Service();" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "    virtual ::grpc::Status SayHello(::grpc::ServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response);" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  };" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  class Stub {" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  public:" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "    virtual ~Stub();" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "    virtual ::grpc::Status SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response);" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "  };" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "};" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "}" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        COMMAND ${CMAKE_COMMAND} -E echo "#endif" >> ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
        COMMENT "Creating stub gRPC files"
    )
endif()

# Create protobuf library
add_library(helloworld_proto ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(helloworld_proto ${Protobuf_LIBRARIES})
target_include_directories(helloworld_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Create gRPC library
add_library(helloworld_grpc ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(helloworld_grpc ${GRPC_LIBRARIES} helloworld_proto)
target_include_directories(helloworld_grpc PUBLIC ${GRPC_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# Server library
add_library(greeter_service
    srv/server.cc
    srv/server.h
)
target_link_libraries(greeter_service helloworld_grpc helloworld_proto)
target_include_directories(greeter_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Server executable
add_executable(server srv/main.cc)
target_link_libraries(server greeter_service)

# Client library
add_library(greeter_client
    cli/client.cc
    cli/client.h
)
target_link_libraries(greeter_client helloworld_grpc helloworld_proto)
target_include_directories(greeter_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Client executable
add_executable(client cli/main.cc)
target_link_libraries(client greeter_client)

# Find Google Test
find_package(GTest QUIET)

if(GTest_FOUND)
    # Client tests
    add_executable(client_test test/cli/client_test.cc)
    target_link_libraries(client_test greeter_client GTest::gtest GTest::gtest_main)
    
    # Server tests
    add_executable(server_test test/srv/server_test.cc)
    target_link_libraries(server_test greeter_service GTest::gtest GTest::gtest_main)
    
    # Integration tests
    add_executable(integration_test test/integration_test.cc)
    target_link_libraries(integration_test greeter_client greeter_service GTest::gtest GTest::gtest_main)
    
    # Enable testing
    enable_testing()
    
    # Add tests
    add_test(NAME client_test COMMAND client_test)
    add_test(NAME server_test COMMAND server_test)
    add_test(NAME integration_test COMMAND integration_test)
    
    message(STATUS "Google Test found - tests enabled")
else()
    message(STATUS "Google Test not found - tests disabled")
endif()

# Installation
install(TARGETS server client
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Protobuf version: ${Protobuf_VERSION}")
message(STATUS "  gRPC version: ${gRPC_VERSION}")
message(STATUS "  Tests enabled: ${GTest_FOUND}")
message(STATUS "")
