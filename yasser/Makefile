PROTO_DIR := proto
GEN_DIR := $(PROTO_DIR)
BIN_DIR := bin

PROTOC ?= protoc
PKG_CONFIG ?= pkg-config

GRPC_CPP_PLUGIN ?= $(shell which grpc_cpp_plugin)
CPPFLAGS := -std=c++17 -O2 -Wall -Wextra
INCLUDES := -I. -I$(PROTO_DIR)

GRPC_CFLAGS := $(shell $(PKG_CONFIG) --cflags protobuf grpc++ 2>/dev/null)
GRPC_LIBS := $(shell $(PKG_CONFIG) --libs protobuf grpc++ 2>/dev/null)

PROTO := $(PROTO_DIR)/hello.proto
GEN_SRCS := $(GEN_DIR)/hello.pb.cc $(GEN_DIR)/hello.grpc.pb.cc
GEN_HDRS := $(GEN_DIR)/hello.pb.h $(GEN_DIR)/hello.grpc.pb.h

CLIENT_SRC := client/main.cc
SERVER_SRC := server/main.cc

CLIENT_BIN := $(BIN_DIR)/client
SERVER_BIN := $(BIN_DIR)/server

.PHONY: all clean deps stubs

# Require grpc C++ plugin to generate stubs
ifeq ($(GRPC_CPP_PLUGIN),)
$(error grpc_cpp_plugin not found. Install protobuf-compiler-grpc (or grpc-tools) and retry)
endif

all: stubs $(CLIENT_BIN) $(SERVER_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(GEN_DIR)/hello.pb.cc $(GEN_DIR)/hello.pb.h: $(PROTO)
	$(PROTOC) -I $(PROTO_DIR) --cpp_out=$(GEN_DIR) $(PROTO)

$(GEN_DIR)/hello.grpc.pb.cc $(GEN_DIR)/hello.grpc.pb.h: $(PROTO) $(GEN_DIR)/hello.pb.h
	$(PROTOC) -I $(PROTO_DIR) --grpc_out=$(GEN_DIR) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN) $(PROTO)

$(CLIENT_BIN): $(BIN_DIR) $(CLIENT_SRC) stubs
	$(CXX) $(CPPFLAGS) $(GRPC_CFLAGS) $(INCLUDES) -o $@ $(CLIENT_SRC) $(GEN_DIR)/hello.pb.cc $(GEN_DIR)/hello.grpc.pb.cc $(GRPC_LIBS)

$(SERVER_BIN): $(BIN_DIR) $(SERVER_SRC) stubs
	$(CXX) $(CPPFLAGS) $(GRPC_CFLAGS) $(INCLUDES) -o $@ $(SERVER_SRC) $(GEN_DIR)/hello.pb.cc $(GEN_DIR)/hello.grpc.pb.cc $(GRPC_LIBS)

# Ensure stubs generated before compiling
stubs: $(GEN_SRCS) $(GEN_HDRS)

clean:
	rm -rf $(BIN_DIR) $(GEN_DIR)/*.pb.cc $(GEN_DIR)/*.pb.h

