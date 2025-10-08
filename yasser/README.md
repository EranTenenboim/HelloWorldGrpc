# Bazel C++ gRPC Hello

This repository contains a minimal C++ gRPC client and server built with Bazel.

## Prerequisites
- Bazel 7+ (bzlmod enabled)
- A C++ toolchain (GCC/Clang)

## Build
```bash
bazel build //server:server //client:client
```

## Run
- Start the server in one terminal:
```bash
bazel run //server:server
```

- In another terminal, run the client:
```bash
bazel run //client:client
```

Expected client output:
```
Hello Client X
```
