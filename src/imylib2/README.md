
## Imylib2

Imylib2 is a wrapper around Imlib2.

Instead of writing code to read a bunch of image formats, it is much easier to use Imlib2 which has already done the work.
Imylib2 takes the loaders from Imlib2 to include directly in Immy, without the extra stuff provided by Imlib2.

### Building

You require [CMake](https://cmake.org/) 3.7 or higher and optionally [GNU Make](https://www.gnu.org/software/make/).

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

This will build a static library `./build/libimylib2.a`.

