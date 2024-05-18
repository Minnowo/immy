# Makefile for doko project

.PHONY: all clean clean_raylib clean_doko rebuild windows

CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

DOKO_SRC_DIR   ?= src
RAYLIB_SRC_DIR ?= raylib/raylib-5.0/src

NAME       := doko
TARGET_DIR ?= build
TARGET     := ${TARGET_DIR}/$(NAME)
RESOURCES  := resources

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

install: CPPFLAGS      += -DDOKO_BUNDLE
install: CFLAGS        += -O3
install: rebuild
	mkdir -p           /opt/doko
	cp    -f $(TARGET) /opt/doko/$(NAME)
	chmod 755          /opt/doko/$(NAME)

	rm -f                   /usr/bin/$(NAME)
	ln    /opt/doko/$(NAME) /usr/bin/$(NAME)

uninstall:
	rm -f /usr/bin/$(NAME)
	rm -f /opt/doko/$(NAME)


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
		CPPFLAGS="$(CPPFLAGS)" \
		LDFLAGS="-L\"$(abspath $(TARGET_DIR))\" -lraylib -lm -lImlib2 $(LDFLAGS)" \
		INCLUDE_PATHS="-I\"$(abspath $(RAYLIB_SRC_DIR))\"" \
		TARGET_DIR="$(abspath $(TARGET_DIR))" 

	cp -r $(RESOURCES) $(TARGET_DIR)
