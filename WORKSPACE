workspace(name = "helloworld_grpc")

# Load rules
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Protocol Buffers - using much older version compatible with Bazel 3.5.1
http_archive(
    name = "protobuf",
    sha256 = "930c2c3b5ecc6c9c12615cf5ad93f1cd6e12d0aba862b572e076259970ac3a53",
    strip_prefix = "protobuf-3.21.12",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v3.21.12.tar.gz"],
)

load("@protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

# gRPC - using much older version compatible with Bazel 3.5.1
http_archive(
    name = "grpc",
    sha256 = "fb1ed98eb3555877d55eb2b948caca44bc8601c6704896594de81558639709ef",
    strip_prefix = "grpc-1.50.1",
    urls = ["https://github.com/grpc/grpc/archive/v1.50.1.tar.gz"],
)

load("@grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()

load("@grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()

# Google Test - using much older version compatible with Bazel 3.5.1
http_archive(
    name = "com_google_googletest",
    sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d8df2",
    strip_prefix = "googletest-1.12.1",
    urls = ["https://github.com/google/googletest/archive/v1.12.1.tar.gz"],
)