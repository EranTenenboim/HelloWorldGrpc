load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "server_test",
    srcs = ["server_test.cc"],
    deps = [
        "//srv:greeter_service",
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)
