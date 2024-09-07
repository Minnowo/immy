
.PHONY: all build build_debug

SOURCE_DIR       := $(abspath .)
BUILD_DIR        := build
CMAKE_BUILD_TYPE := Release

all: build

build_debug: CMAKE_BUILD_TYPE := Debug
build_debug: build

build:
	CC="$(CC)" CXX="$(CXX)" cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	CC="$(CC)" CXX="$(CXX)" make -C  $(BUILD_DIR) -j$(shell nproc)

