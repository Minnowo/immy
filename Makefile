# Makefile for doko project

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c99

DOKO_SRC_DIR := src
RAYLIB_SRC_DIR := raylib/raylib-5.0/src

BUILD_DIR := build
RAYLIB_TARGET_DIR := ${BUILD_DIR}/raylib
DOKO_TARGET_DIR := ${BUILD_DIR}/

TARGET := ${BUILD_DIR}/doko


all: doko_ raylib_ ${TARGET}

doko_:
	$(MAKE) -C $(DOKO_SRC_DIR) BUILD_DIR=$(abspath $(BUILD_DIR)) CFLAGS="$(CFLAGS)" CC="$(CC)"

raylib_:
	$(MAKE) -C $(RAYLIB_SRC_DIR)
	mkdir -p $(RAYLIB_TARGET_DIR)
	cp $(RAYLIB_SRC_DIR)/*.o $(RAYLIB_TARGET_DIR)

${TARGET}:
	${CC} -o $@ build/*.o build/raylib/*.o  -lm

clean_doko:
	rm -rf $(TARGET) ${BUILD_DIR}/*.o

clean:
	$(MAKE) -C $(RAYLIB_SRC_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: all clean