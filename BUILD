# HelloWorld gRPC project BUILD file
# This file defines the proto and gRPC libraries for the HelloWorld service

load("@rules_proto_grpc//cpp:defs.bzl", "cpp_grpc_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(default_visibility = ["//visibility:public"])

# Protocol buffer definition for HelloWorld service
proto_library(
    name = "helloworld_proto",
    srcs = ["proto/helloworld.proto"],
    deps = [],
)

# C++ protocol buffer library
cc_proto_library(
    name = "helloworld_cc_proto",
    deps = [":helloworld_proto"],
)

# gRPC C++ library
cpp_grpc_library(
    name = "helloworld_grpc",
    protos = [":helloworld_proto"],
    deps = [":helloworld_cc_proto"],
)
