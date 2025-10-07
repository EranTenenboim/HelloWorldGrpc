# HelloWorldGrpc

## Build and Run with Make

Prerequisites:
- g++ (C++17)
- CMake >= 3.16
- Protobuf (`protoc`, `libprotobuf-dev`)
- gRPC C++ development libraries (e.g., `libgrpc++-dev`, `grpc_cpp_plugin`)

### Build

```bash
make build
```

Artifacts are placed under `build/`:
- `build/hello_server`
- `build/hello_client`

### Run

Start the server (runs in background and logs to `build/hello_server.log`):

```bash
make run-server
```

Run the client:

```bash
make run-client
```

Stop the server:

```bash
make stop-server
```

### Clean

Remove the entire CMake build directory:

```bash
make clean
```

## Build and Run with Bazel

Prerequisites:
- Bazel installed (`bazel` on PATH)
- System `protoc`, `grpc_cpp_plugin`, and gRPC/protobuf dev libs available to linker

This repo includes a minimal, non-hermetic Bazel setup that shells out to system `protoc` and `grpc_cpp_plugin`.

### Build

```bash
bazel build //:hello_server //:hello_client
```

### Run

```bash
./bazel-bin/hello_server
./bazel-bin/hello_client
```

Notes:
- If the link step fails, ensure `libgrpc++`/`libgrpc`/`libprotobuf` are installed (e.g., `sudo apt install libgrpc++-dev protobuf-compiler-grpc`).
- For fully hermetic builds, consider switching to `rules_proto` + `rules_proto_grpc` in `WORKSPACE` and `BUILD`.
