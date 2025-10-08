# Server BUILD file
# This file defines the server binary target

load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

# Server binary with gRPC
cc_binary(
    name = "server",
    srcs = glob(["*.cc"]),
    deps = [
        "//:cc_proto",
        "//:grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_protobuf//:protobuf",
    ],
    copts = [
        "-std=c++17",
    ],
)
