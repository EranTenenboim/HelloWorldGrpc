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

# Find gRPC (with fallback for missing plugin)
find_package(gRPC QUIET)
if(NOT gRPC_FOUND)
    message(WARNING "gRPC not found, trying alternative approach...")
    # Try to find gRPC components manually
    find_library(GRPC_LIBRARY grpc++)
    find_library(GRPC_UNSECURE_LIBRARY grpc++_unsecure)
    if(GRPC_LIBRARY)
        message(STATUS "Found gRPC library: ${GRPC_LIBRARY}")
    else()
        message(FATAL_ERROR "gRPC not found. Please install libgrpc++-dev")
    endif()
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
if(TARGET gRPC::grpc_cpp_plugin)
    add_custom_command(
        OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
        COMMAND protoc
        ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
             --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
             --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
             ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/helloworld.proto
        COMMENT "Generating gRPC files"
    )
else()
    # Fallback: try to find grpc_cpp_plugin in PATH
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
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
        message(WARNING "grpc_cpp_plugin not found, skipping gRPC generation")
        # Create empty files to avoid build errors
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc "// gRPC files not generated\n")
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h "// gRPC files not generated\n")
    endif()
endif()

# Create protobuf library
add_library(helloworld_proto ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(helloworld_proto ${Protobuf_LIBRARIES})
target_include_directories(helloworld_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Create gRPC library
add_library(helloworld_grpc ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(helloworld_grpc gRPC::grpc++ helloworld_proto)
target_include_directories(helloworld_grpc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

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
