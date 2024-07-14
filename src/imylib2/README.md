
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

---

**Note:** When Imylib2 is built, CMake will download Imlib2 sources from SourceForge. 
It does not install anything, just be aware it will be making a network call for this.

If you do not want CMake to download Imlib2 you can add it yourself:
1. Create a folder `Imlib2` in this directory
2. Download Imlib2 ([SourceForge](https://sourceforge.net/projects/enlightenment/files/imlib2-src/))
3. Extract the tar.gz (somewhere, doesn't matter)
4. In the extacted source, run the `./configure` script
5. Copy `<imlib2-extract>/src/modules/loaders/` directory into the `Imlib2` folder from step 1
6. Copy `<imlib2-extract>/src/lib/Imlib2_Loader.h` into the `Imlib2` folder from step 1
7. Copy `<imlib2-extract>/config.h` into the `Imlib2/loaders/` folder from step 5
8. Open the `./CMakeLists.txt` and search for `MANUAL_IMLIB2`, then change to `ON`
9. Build as normal

---

