load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "greeter_client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "client",
    srcs = ["main.cc"],
    deps = [
        ":greeter_client",
    ],
)

