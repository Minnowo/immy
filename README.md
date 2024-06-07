
## A Simple Image Viewer

doko is a simple, keyboard focused image viewer for Linux. Built using [Raylib](https://github.com/raysan5/raylib), it has a minimal and fast interface, and supports most common image formats. Doko can optionally call ImageMagick and FFmpeg to support many more image formats.


### Configuration

All settings which are easily configurable are located in `src/config.h`.


### Building

You require [CMake](https://cmake.org/) 3.7 or higher and [GNU Make](https://www.gnu.org/software/make/).

Using the Makefile (which will call cmake):
```sh
make build
```

Using CMake directly:
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

This will bundle the resources and build everything. Your built executable will be `./build/doko`.


#### Windows

It is possible to compile for Widnows, but I haven't tried since switching to cmake.

### Install

First build doko using the build instructions. Then run `sudo make install`.

### Uninstall

On Linux systems, calling `sudo make uninstall` which should fully uninstall everything.

You can alternatively delete the files manually:
- Main directory from `/opt/doko`
- The symlinks `/usr/bin/doko` and `/usr/bin/im`.
- The Desktop file `/usr/share/applications/doko.desktop` and then run `update-desktop-database /usr/share/applications/`

