cmake_minimum_required(VERSION 3.16)
project(hello_grpc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)

# Try to locate gRPC headers and libraries directly (Ubuntu libgrpc++-dev)
find_path(GRPC_INCLUDE_DIR grpcpp/grpcpp.h
  PATHS /usr/include /usr/local/include)
find_library(GRPCPP_LIBRARY NAMES grpc++ PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
find_library(GRPC_LIBRARY NAMES grpc PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
set(GRPC_INCLUDE_DIRS ${GRPC_INCLUDE_DIR})
set(GRPC_LIBRARIES ${GRPCPP_LIBRARY} ${GRPC_LIBRARY})
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin REQUIRED)
if(NOT Protobuf_PROTOC_EXECUTABLE)
  find_program(Protobuf_PROTOC_EXECUTABLE NAMES protoc REQUIRED)
endif()
include_directories(${GRPC_INCLUDE_DIRS})

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_FILE ${PROTO_SRC_DIR}/helloworld.proto)

set(GENERATED_PROTO_SRCS
    ${CMAKE_BINARY_DIR}/generated/helloworld.pb.cc
    ${CMAKE_BINARY_DIR}/generated/helloworld.grpc.pb.cc)
set(GENERATED_PROTO_HDRS
    ${CMAKE_BINARY_DIR}/generated/helloworld.pb.h
    ${CMAKE_BINARY_DIR}/generated/helloworld.grpc.pb.h)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

add_custom_command(
  OUTPUT ${GENERATED_PROTO_SRCS} ${GENERATED_PROTO_HDRS}
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    --proto_path=${PROTO_SRC_DIR}
    --cpp_out=${CMAKE_BINARY_DIR}/generated
    --grpc_out=${CMAKE_BINARY_DIR}/generated
    --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
    ${PROTO_FILE}
  DEPENDS ${PROTO_FILE}
  COMMENT "Generating C++ sources from proto"
)

add_library(helloworld_protos
  ${GENERATED_PROTO_SRCS}
  ${GENERATED_PROTO_HDRS}
)
target_include_directories(helloworld_protos PUBLIC ${CMAKE_BINARY_DIR}/generated ${Protobuf_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
target_link_libraries(helloworld_protos PUBLIC ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(hello_server srv/server.cc)
target_include_directories(hello_server PRIVATE ${CMAKE_BINARY_DIR}/generated ${GRPC_INCLUDE_DIRS})
target_link_libraries(hello_server PRIVATE helloworld_protos ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(hello_client cli/client.cc)
target_include_directories(hello_client PRIVATE ${CMAKE_BINARY_DIR}/generated ${GRPC_INCLUDE_DIRS})
target_link_libraries(hello_client PRIVATE helloworld_protos ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})

add_dependencies(hello_server helloworld_protos)
add_dependencies(hello_client helloworld_protos)

# Locate grpc_cpp_plugin executable if not found via CMake package
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin REQUIRED)


