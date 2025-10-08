# Master BUILD file template for gRPC projects
# This file should be customized by each project to define its own structure
# 
# Example usage:
# 1. Copy this file to your project
# 2. Define your proto files location
# 3. Define your server/client source locations
# 4. Customize target names as needed

load("@rules_proto_grpc//cpp:defs.bzl", "cpp_grpc_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library", "cc_library")

package(default_visibility = ["//visibility:public"])

# TODO: Replace with your actual proto file paths
# Example: srcs = glob(["proto/*.proto"]) or srcs = ["my_service.proto"]
proto_library(
    name = "proto",
    srcs = [],  # <-- Define your proto files here
    deps = [
        "@com_google_protobuf//:any_proto",
        "@com_google_protobuf//:timestamp_proto",
        "@com_google_protobuf//:empty_proto",
    ],
)

# TODO: Add any additional proto dependencies
# Example: deps = ["@com_google_protobuf//:any_proto"]
cc_proto_library(
    name = "cc_proto",
    deps = [":proto"],
)

# TODO: Define your gRPC generated files location
# Example: Use cpp_grpc_library for automatic generation
cpp_grpc_library(
    name = "grpc",
    protos = [":proto"],
    deps = [":cc_proto"],
)

# TODO: Create BUILD files in your server and client directories
# Example: Create srv/BUILD and cli/BUILD files with cc_binary targets
# The binaries should depend on the proto libraries defined above:
#   deps = [":cc_proto", ":grpc"]
#
# See BUILD.example for a complete example of how to structure this.
