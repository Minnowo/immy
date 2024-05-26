
## A Simple Image Viewer

Doko is a simple, keyboard focused image viewer for Linux*. Built using [Raylib](https://github.com/raysan5/raylib), it has a minimal and fast interface, and supports most common image formats. Doko can optionally call ImageMagick and FFmpeg to support many more image formats.

It mainly targets Linux but can be cross compiled for Windows as well.


### Configuration

All settings which are easily configurable are located in `src/config.h`.


### Building

This repository contains all the code required for building Doko as the Raylib version is baked right in.

Using the legacy Makefile, simply call `make` from the root directory to build an executable. In order to comiple the resources directly into the binary, you will need to pass the `-DDOKO_BUNDLE` flag. This can be done by adding the value to the `CPPFLAGS` in the Makefile. It is automatically set when running `make install`.

Using CMake:
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

This will bundle the resources and build everything. Your built executable will be `./build/doko`.


#### Windows

I tried to make it easy to cross compile for Windows in the Makefile.

Make sure you have MinGW installed, and run `make windows` to attempt to cross compile to target Windows.

**Note**: As of right now, Doko cannot make calls to ImageMagick or FFmpeg when compiled for Windows. This will limit the image format support to whatever Raylib supports.


### Install

On Linux systems, calling `make install` will build and place Doko in `/opt/doko/`. 

### Uninstall

On Linux systems, calling `make uninstall` will uninstall the program.

You can alternatively delete the files manually from `/opt/doko` and the symlinks `/usr/bin/doko` and `/usr/bin/im`.
