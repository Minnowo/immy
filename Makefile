# Makefile for doko project

.PHONY: all build build_debug

SOURCE_DIR       := $(abspath .)
BUILD_DIR        := build
CMAKE_BUILD_TYPE := Release

NAME       := doko
NAME_ALT   := im
TARGET     := ${BUILD_DIR}/$(NAME)

all: build

build_debug: CMAKE_BUILD_TYPE := Debug
build_debug: build

build:
	mkdir -p $(BUILD_DIR)
	cd       $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) $(SOURCE_DIR)
	make -C  $(BUILD_DIR)

install: uninstall
	mkdir -p           /opt/$(NAME)
	cp    -f $(TARGET) /opt/$(NAME)/$(NAME)
	chmod 755          /opt/$(NAME)/$(NAME)

	rm -f                   /usr/bin/$(NAME)
	ln    /opt/$(NAME)/$(NAME) /usr/bin/$(NAME)
	ln    /opt/$(NAME)/$(NAME) /usr/bin/$(NAME_ALT)

	desktop-file-install    --dir=/usr/share/applications/ ./resources/doko.desktop
	update-desktop-database       /usr/share/applications/

uninstall:
	rm -f /usr/bin/$(NAME)
	rm -f /usr/bin/$(NAME_ALT)
	rm -f /opt/$(NAME)/$(NAME)
	rm -f /usr/share/applications/doko.desktop

