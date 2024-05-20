# Makefile for doko project

.PHONY: all build clean clean_raylib clean_doko rebuild windows build_doko build_raylib

CC     ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99

DOKO_SRC_DIR   ?= src
RAYLIB_SRC_DIR ?= raylib/raylib-5.0/src

NAME       := doko
NAME_ALT   := im
TARGET_DIR ?= build
TARGET     := ${TARGET_DIR}/$(NAME)
RESOURCES  := resources

# RAYLIB_LIBTYPE ?= STATIC
# PLATFORM_OS    ?= LINUX

ifeq ($(PLATFORM_OS), WINDOWS)
	LDFLAGS += -lopengl32 -lgdi32 -lwinmm
endif

all: build

clean: clean_doko clean_raylib

build: build_raylib build_doko

windows: CC          := x86_64-w64-mingw32-gcc
windows: PLATFORM_OS := WINDOWS
windows: CFLAGS      += -Wl,--subsystem,windows     # remove console on windows
windows: LDFLAGS     += -lopengl32 -lgdi32 -lwinmm
windows: all

rebuild: clean_doko all

install: CPPFLAGS      += -DDOKO_BUNDLE
install: CFLAGS        += -O3
install: uninstall build 
	mkdir -p           /opt/doko
	cp    -f $(TARGET) /opt/doko/$(NAME)
	chmod 755          /opt/doko/$(NAME)

	rm -f                   /usr/bin/$(NAME)
	ln    /opt/doko/$(NAME) /usr/bin/$(NAME)
	ln    /opt/doko/$(NAME) /usr/bin/$(NAME_ALT)

	desktop-file-install    --dir=/usr/share/applications/ ./resources/doko.desktop
	update-desktop-database       /usr/share/applications/

uninstall:
	rm -f /usr/bin/$(NAME)
	rm -f /usr/bin/$(NAME_ALT)
	rm -f /opt/doko/$(NAME)
	rm -f /usr/share/applications/doko.desktop

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

clean_raylib:
	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))" \
		clean

build_raylib: | $(TARGET_DIR)

	$(MAKE) -C $(RAYLIB_SRC_DIR) \
		CC="$(CC)" \
		PLATFORM=PLATFORM_DESKTOP \
		PLATFORM_OS="$(PLATFORM_OS)" \
		RAYLIB_LIBTYPE="$(RAYLIB_LIBTYPE)" \
		RAYLIB_RELEASE_PATH="$(abspath $(TARGET_DIR))"

clean_doko:
	$(MAKE) -C $(DOKO_SRC_DIR) \
		TARGET_DIR="$(abspath $(TARGET_DIR))" \
		clean

build_doko: | $(TARGET_DIR)

	$(MAKE) -C $(DOKO_SRC_DIR) \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS)" \
		CPPFLAGS="$(CPPFLAGS)" \
		LDFLAGS="-L\"$(abspath $(TARGET_DIR))\" -lraylib -lm -lImlib2 $(LDFLAGS)" \
		INCLUDE_PATHS="-I\"$(abspath $(RAYLIB_SRC_DIR))\"" \
		TARGET_DIR="$(abspath $(TARGET_DIR))" 

	cp -r $(RESOURCES) $(TARGET_DIR)

