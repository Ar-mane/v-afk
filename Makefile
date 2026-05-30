APP_NAME := V-AFK
BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/Debug
RELEASE_DIR := $(BUILD_DIR)/Release

.PHONY: help configure build build-debug build-release kill run format clean

help:
	@echo "Basic targets:"
	@echo "  make configure     - generate CMake project files"
	@echo "  make build         - build Release"
	@echo "  make build-debug   - build Debug"
	@echo "  make build-release - build Release"
	@echo "  make run           - stop running app, then launch Release exe"
	@echo "  make kill          - stop any running V-AFK session"
	@echo "  make format        - format src/ (clang-format, like prettier)"
	@echo "  make clean         - remove build folder"

format:
	  ./clang-format -i src/*.cpp src/*.h src/ui/*.cpp src/ui/*.h
 
configure:
	cmake -S . -B $(BUILD_DIR)

build: build-release

build-debug: configure
	cmake --build $(BUILD_DIR) --config Debug

build-release: configure
	cmake --build $(BUILD_DIR) --config Release

kill:
	-powershell -NoProfile -Command "Stop-Process -Name '$(APP_NAME)' -Force -ErrorAction SilentlyContinue"

run: kill build-release 
	powershell -NoProfile -Command "Start-Process -FilePath './$(RELEASE_DIR)/$(APP_NAME).exe'"

clean:
	rm -rf $(BUILD_DIR)

