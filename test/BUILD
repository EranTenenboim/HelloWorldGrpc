load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "integration_test",
    srcs = ["integration_test.cc"],
    deps = [
        "//cli:greeter_client",
        "//srv:greeter_service",
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)

# Test suite that runs all tests
cc_test(
    name = "all_tests",
    srcs = [],
    deps = [
        "//test/cli:client_test",
        "//test/srv:server_test", 
        "//test:integration_test",
    ],
    testonly = True,
    visibility = ["//visibility:public"],
)
