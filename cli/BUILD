# Client BUILD file
# This file defines the client binary target

load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

# Client binary with gRPC
cc_binary(
    name = "client",
    srcs = glob(["*.cc"]),
    deps = [
        "//:helloworld_cc_proto",
        "//:helloworld_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_protobuf//:protobuf",
    ],
    copts = [
        "-std=c++17",
    ],
)
