load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "client_test",
    srcs = ["client_test.cc"],
    deps = [
        "//cli:greeter_client",
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)
