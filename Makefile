SHELL := /usr/bin/bash

.PHONY: all build rebuild clean run-server stop-server run-client

PROJECT_DIR := $(abspath .)
BUILD_DIR := $(PROJECT_DIR)/build
SERVER_BIN := $(BUILD_DIR)/hello_server
CLIENT_BIN := $(BUILD_DIR)/hello_client
SERVER_PID_FILE := $(BUILD_DIR)/hello_server.pid
SERVER_LOG_FILE := $(BUILD_DIR)/hello_server.log

all: build

build:
	cmake -S $(PROJECT_DIR) -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -- -j

rebuild: clean build

clean:
	rm -rf $(BUILD_DIR)

# Start server in background, capture PID, and log output
run-server: build
	@mkdir -p $(BUILD_DIR)
	@echo "Starting server..."
	@nohup $(SERVER_BIN) > $(SERVER_LOG_FILE) 2>&1 & echo $$! > $(SERVER_PID_FILE)
	@echo "Server started (PID $$(cat $(SERVER_PID_FILE))). Logs: $(SERVER_LOG_FILE)"

stop-server:
	@if [ -f $(SERVER_PID_FILE) ]; then \
		PID=$$(cat $(SERVER_PID_FILE)); \
		if ps -p $$PID > /dev/null 2>&1; then \
			echo "Stopping server (PID $$PID)..."; \
			kill $$PID; \
			wait $$PID 2>/dev/null || true; \
		else \
			echo "Server process $$PID not running"; \
		fi; \
		rm -f $(SERVER_PID_FILE); \
	else \
		echo "No PID file found at $(SERVER_PID_FILE)"; \
	fi

run-client: build
	$(CLIENT_BIN)


