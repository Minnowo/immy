# Makefile for doko project

.PHONY: all clean clean_raylib clean_doko

CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

DOKO_SRC_DIR   ?= src
RAYLIB_SRC_DIR ?= raylib/raylib-5.0/src

TARGET_DIR ?= build
TARGET     := ${TARGET_DIR}/doko

all: ${TARGET}

clean: clean_doko clean_raylib

clean_doko:
	$(MAKE) -C $(DOKO_SRC_DIR) \
		TARGET_DIR="$(abspath $(TARGET_DIR))" \
		clean

clean_raylib:
	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))" \
		clean

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

${TARGET}: | $(TARGET_DIR)

	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		CC="$(CC)" \
		PLATFORM=PLATFORM_DESKTOP \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))"

	$(MAKE) -C $(DOKO_SRC_DIR) \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="-L\"$(abspath $(TARGET_DIR))\" -lraylib -lm" \
		TARGET_DIR="$(abspath $(TARGET_DIR))" \
		INCLUDE_PATHS=-I"$(abspath $(RAYLIB_SRC_DIR))"

