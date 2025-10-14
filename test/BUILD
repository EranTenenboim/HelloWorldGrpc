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
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "ptp_test",
    srcs = ["ptp_test.cc"],
    deps = [
        "//cli:greeter_client",
        "//srv:greeter_service",
        "//proto:helloworld_cc_proto",
        "//proto:helloworld_grpc_cc_proto",
        "@grpc//:grpc++",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    visibility = ["//visibility:public"],
)

# Test suite that runs all tests - using test_suite instead of cc_test
test_suite(
    name = "all_tests",
    tests = [
        "//test/cli:client_test",
        "//test/srv:server_test", 
        "//test:integration_test",
        "//test:ptp_test",
    ],
    visibility = ["//visibility:public"],
)
