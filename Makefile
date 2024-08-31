
.PHONY: all build build_debug

SOURCE_DIR       := $(abspath .)
BUILD_DIR        := build
CMAKE_BUILD_TYPE := Release

NAME       := immy
NAME_ALT   := im
TARGET     := ${BUILD_DIR}/$(NAME)

all: build

build_debug: CMAKE_BUILD_TYPE := Debug
build_debug: build

wayland: DESKTOP_TARGET := Wayland
wayland: build

x11: DESKTOP_TARGET := X11
x11: build

build:
	CC="$(CC)" CXX="$(CXX)" cmake -B $(BUILD_DIR) \
	   -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
	   -DDESKTOP_TARGET=$(DESKTOP_TARGET)
	CC="$(CC)" CXX="$(CXX)" make -C  $(BUILD_DIR) -j$(shell nproc)
	cp $(BUILD_DIR)/compile_commands.json $(SOURCE_DIR) || true

install: uninstall
	mkdir -p             "/opt/$(NAME)"
	cp    -f "$(TARGET)" "/opt/$(NAME)/$(NAME)"
	chmod 755            "/opt/$(NAME)/$(NAME)"
	rm -f "/usr/bin/$(NAME)"
	ln    "/opt/$(NAME)/$(NAME)" "/usr/bin/$(NAME)"
	ln    "/opt/$(NAME)/$(NAME)" "/usr/bin/$(NAME_ALT)"

	desktop-file-install    --dir=/usr/share/applications/ ./resources/immy.desktop
	update-desktop-database       /usr/share/applications/

uninstall:
	rm -f "/usr/bin/$(NAME)"
	rm -f "/usr/bin/$(NAME_ALT)"
	rm -f "/opt/$(NAME)/$(NAME)"
	rm -f "/usr/share/applications/immy.desktop"
	update-desktop-database /usr/share/applications/
	rmdir "/opt/$(NAME)" || true

setup_cross_compile:
	git clone "https://github.com/dockcross/dockcross.git"
	cd dockcross && docker run --rm dockcross/windows-static-x64-posix > ./dockcross-windows-static-x64-posix
	chmod +x dockcross/dockcross-windows-static-x64-posix

