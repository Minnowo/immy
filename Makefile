# Makefile for doko project

.PHONY: all clean clean_raylib clean_doko rebuild windows

CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

DOKO_SRC_DIR   ?= src
RAYLIB_SRC_DIR ?= raylib/raylib-5.0/src

NAME       := doko
TARGET_DIR ?= build
TARGET     := ${TARGET_DIR}/$(NAME)

# RAYLIB_LIBTYPE ?= STATIC
# PLATFORM_OS    ?= LINUX

ifeq ($(PLATFORM_OS), WINDOWS)
	LDFLAGS += -lopengl32 -lgdi32 -lwinmm
endif

all: ${TARGET}

clean: clean_doko clean_raylib

windows: CC          := x86_64-w64-mingw32-gcc
windows: PLATFORM_OS := WINDOWS
windows: CFLAGS      += -Wl,--subsystem,windows     # remove console on windows
windows: LDFLAGS     += -lopengl32 -lgdi32 -lwinmm
windows: all

rebuild: clean_doko all

clean_doko:
	$(MAKE) -C $(DOKO_SRC_DIR) \
		TARGET_DIR="$(abspath $(TARGET_DIR))" \
		clean

clean_raylib:
	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))" \
		clean

install: $(TARGET)
	mkdir -p /usr/local/bin
	cp -f $(TARGET) /usr/local/bin/$(NAME)
	chmod 755 /usr/local/bin/$(NAME)

uninstall:
	rm -f /usr/local/bin/$(NAME)


$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

${TARGET}: | $(TARGET_DIR)

	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		CC="$(CC)" \
		PLATFORM=PLATFORM_DESKTOP \
		PLATFORM_OS="$(PLATFORM_OS)" \
		RAYLIB_LIBTYPE="$(RAYLIB_LIBTYPE)" \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))"

	$(MAKE) -C $(DOKO_SRC_DIR) \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="-L\"$(abspath $(TARGET_DIR))\" -lraylib -lm $(LDFLAGS)" \
		INCLUDE_PATHS="-I\"$(abspath $(RAYLIB_SRC_DIR))\"" \
		TARGET_DIR="$(abspath $(TARGET_DIR))" 

