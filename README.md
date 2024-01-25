
## A Simple Image Viewer

Doko is a simple, keyboard focused image viewer for Linux*. The GUI is powered by [Raylib](https://github.com/raysan5/raylib) which provides a minimal and fast interface.

It mainly targets Linux but can be cross compiled for Windows as well.


### Configuration

All settings which are easily configurable are located in `src/config.h`.


### Building

This repository contains all the code required for building Doko as the Raylib version is baked right in.

Clone the repo and call `make` from the root directory. This should produce a binary `bin/doko`.

**Note**: The produced binary will search for resources the run directory. You can set the path it searches at compile-time using the `RESOURCE_PATH` (located in the `src/config.h`) which will be the root directory it searches in. (fixing this is a TODO)

#### Windows

I tried to make it easy to cross compile for Windows in the Makefile.

Make sure you have MinGW installed, and run `make windows` to attempt to cross compile to target Windows.

**Note**: As of right now, Doko cannot make calls to ImageMagick or FFmpeg when compiled for Windows. This will limit the image format support to whatever Raylib supports.


### Install

On Linux systems, calling `make install` will build and place Doko in `/opt/doko/`. This will automatically set the `RESOURCE_PATH` to this folder to prevent problems finding fonts. It will then create a symlink to `/usr/bin/doko` which should hopefully be in path.


### Uninstall

On Linux systems, calling `make uninstall` will uninstall the program.

You can alternatively delete the files manually from `/opt/doko` and the file `/usr/bin/doko`.