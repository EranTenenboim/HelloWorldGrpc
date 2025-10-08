package(default_visibility = ["//visibility:public"])

# Protocol buffer definition
proto_library(
    name = "helloworld_proto",
    srcs = glob(["proto/*.proto"]),
)

# Protocol buffer C++ library
cc_proto_library(
    name = "helloworld_cc_proto",
    deps = [":helloworld_proto"],
)

# gRPC C++ library - we'll generate this manually for now
cc_library(
    name = "helloworld_grpc",
    srcs = glob(["proto/*.grpc.pb.cc"], allow_empty = True),
    hdrs = glob(["proto/*.grpc.pb.h"], allow_empty = True),
    includes = ["proto"],
    deps = [":helloworld_cc_proto"],
)

# Server binary with gRPC
cc_binary(
    name = "hello_server",
    srcs = glob(["srv/*.cc"]),
    deps = [
        ":helloworld_cc_proto",
        ":helloworld_grpc",
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = [
        "-lgrpc++",
        "-lgrpc",
        "-lprotobuf",
        "-lpthread",
    ],
)

# Client binary with gRPC
cc_binary(
    name = "hello_client",
    srcs = glob(["cli/*.cc"]),
    deps = [
        ":helloworld_cc_proto",
        ":helloworld_grpc",
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = [
        "-lgrpc++",
        "-lgrpc",
        "-lprotobuf",
        "-lpthread",
    ],
)
