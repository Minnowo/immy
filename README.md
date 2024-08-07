
## Immy

A simple, keyboard focused image viewer for Linux. 

Built using [Raylib](https://github.com/raysan5/raylib) with a minimal interface inspired by [sxiv](https://github.com/xyb3rt/sxiv).
Immy primarily makes use of Imlib2 through a [custom library](./src/imylib2/), but can also use ImageMagick, FFmpeg or Raylib to load images.

Configuration is handled using a single header file: `src/config.h`.


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

This will bundle the resources and build everything. Your built executable will be `./build/immy`.

---

**Note:** When Imylib2 is built, CMake will download Imlib2 sources from SourceForge. 
If you do not want this, you can add Imlib2 sources yourself.
See the note in [Imylib2](./src/imylib2/README.md) about this.

---

#### Windows

Immy can be cross compiled for Windows. 
I recommend using [Dockcross](https://github.com/dockcross/dockcross), 
which provides a Docker environment for cross compiling.

To compile a static binary for Windows:
```sh
# clone the Dockcross repo
git clone https://github.com/dockcross/dockcross.git

# enter the repo
cd dockcross 

# create the helper binary
docker run --rm dockcross/windows-static-x64-posix > ./dockcross-windows-static-x64-posix

# make it executable
chmod +x dockcross/dockcross-windows-static-x64-posix

# build the binary
./dockcross/dockcross-windows-static-x64-posix make build
```

This will bundle the resources and build everything. Your built executable will be `./build/immy.exe`.


### Install

First build Immy using the build instructions. Then run `sudo make install`.

### Uninstall

On Linux systems, calling `sudo make uninstall` which should fully uninstall everything.

You can alternatively delete the files manually:
- Main directory from `/opt/immy`
- The symlinks `/usr/bin/immy` and `/usr/bin/im`.
- The Desktop file `/usr/share/applications/immy.desktop` and then run `update-desktop-database /usr/share/applications/`

