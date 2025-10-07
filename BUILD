# Minimal Bazel setup that uses system-installed protoc and grpc_cpp_plugin.
# This is a pragmatic, non-hermetic setup. For hermetic builds, consider rules_proto + rules_proto_grpc.

package(default_visibility = ["//visibility:public"])

# Generate C++ sources from proto using protoc and grpc_cpp_plugin on PATH

genrule(
    name = "generate_protos",
    srcs = ["proto/helloworld.proto"],
    outs = [
        "generated/helloworld.pb.cc",
        "generated/helloworld.pb.h",
        "generated/helloworld.grpc.pb.cc",
        "generated/helloworld.grpc.pb.h",
    ],
    cmd = "mkdir -p $(@D)/generated && /usr/bin/protoc --proto_path=$$(dirname $(location proto/helloworld.proto)) --cpp_out=$(@D)/generated --grpc_out=$(@D)/generated --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin $(location proto/helloworld.proto)",
)

cc_library(
    name = "helloworld_protos",
    srcs = [
        "generated/helloworld.pb.cc",
        "generated/helloworld.grpc.pb.cc",
    ],
    hdrs = [
        "generated/helloworld.pb.h",
        "generated/helloworld.grpc.pb.h",
    ],
    includes = ["generated"],
)

cc_binary(
    name = "hello_server",
    srcs = ["srv/server.cc"],
    deps = [":helloworld_protos"],
    linkopts = [
        "-lgrpc++",
        "-lgrpc",
        "-lprotobuf",
    ],
)

cc_binary(
    name = "hello_client",
    srcs = ["cli/client.cc"],
    deps = [":helloworld_protos"],
    linkopts = [
        "-lgrpc++",
        "-lgrpc",
        "-lprotobuf",
    ],
)
