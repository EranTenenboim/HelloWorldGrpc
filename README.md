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
