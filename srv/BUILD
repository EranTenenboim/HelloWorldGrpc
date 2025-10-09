load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "greeter_service",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "server",
    srcs = ["main.cc"],
    deps = [
        ":greeter_service",
    ],
)

