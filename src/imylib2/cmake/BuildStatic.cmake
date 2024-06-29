

set(STATIC_LIBS_DIR ${CMAKE_BINARY_DIR}/static_libs)

include(ExternalProject)

ExternalProject_Add(
    libwebp
    URL https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-1.4.0.tar.gz
    URL_MD5 07daaa13bef03b7be07f11977b159cb8
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                      cmake
                      -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                      -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                      -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                      -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                      -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                      -DCMAKE_BUILD_TYPE=RELEASE
                      -DWEBP_LINK_STATIC=ON
                      -DWEBP_BUILD_CWEBP=OFF
                      -DWEBP_BUILD_DWEBP=OFF
                      -DWEBP_BUILD_GIF2WEBP=OFF
                      -DWEBP_BUILD_IMG2WEBP=OFF
                      -DWEBP_BUILD_VWEBP=OFF
                      -DWEBP_BUILD_WEBPINFO=OFF
                      -DWEBP_BUILD_WEBPMUX=OFF
                      -DWEBP_BUILD_WEBP_JS=OFF
                      -DWEBP_BUILD_FUZZTEST=OFF
                      -DWEBP_USE_THREAD=ON
                      -DWEBP_BUILD_LIBWEBPMUX=ON
                      ..
    BUILD_COMMAND cd build && make
    INSTALL_COMMAND cd build && make install
)

ExternalProject_Add(
    libpng
    URL https://downloads.sourceforge.net/libpng/libpng-1.6.43.tar.xz
    URL_MD5 22b8362d16c3724eba9c1fb8d187320a
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                      cmake
                      -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                      -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                      -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                      -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                      -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                      -DCMAKE_BUILD_TYPE=RELEASE
                      -DPNG_STATIC=ON
                      -DPNG_SHARED=OFF
                      -DPNG_TESTS=OFF
                      -DPNG_EXECUTABLES=OFF
                      ..
    BUILD_COMMAND cd build && make
    INSTALL_COMMAND cd build && make install
)

ExternalProject_Add(
    libjpeg
    URL https://downloads.sourceforge.net/libjpeg-turbo/libjpeg-turbo-3.0.1.tar.gz
    URL_MD5 1fdc6494521a8724f5f7cf39b0f6aff3
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                      cmake
                      -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                      -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                      -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                      -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                      -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                      -DCMAKE_BUILD_TYPE=RELEASE
                      -DENABLE_STATIC=ON
                      -DENABLE_SHARED=OFF
                      -DWITH_JAVA=OFF
                      -DCMAKE_SKIP_INSTALL_RPATH=ON
                      ..
    BUILD_COMMAND cd build && make
    INSTALL_COMMAND cd build && make install
)

ExternalProject_Add(
    libtiff
    URL https://download.osgeo.org/libtiff/tiff-4.6.0.tar.gz
    URL_MD5 fc7d49a9348b890b29f91a4ecadd5b49
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                      cmake
                      -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                      -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                      -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                      -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                      -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                      -DCMAKE_BUILD_TYPE=RELEASE
                      -DBUILD_SHARED_LIBS=OFF
                      -Dtiff-tools=OFF
                      -Dtiff-tests=OFF
                      -Dtiff-contrib=OFF
                      -Dtiff-docs=OFF
                      ..
    BUILD_COMMAND cd build && make
    INSTALL_COMMAND cd build && make install
)

ExternalProject_Add(
    libopenjpeg
    URL https://github.com/uclouvain/openjpeg/archive/v2.4.0/openjpeg-2.4.0.tar.gz
    URL_MD5 4d388298335947367e91f1d100468af1
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                      cmake
                      -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                      -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                      -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                      -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                      -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                      -DCMAKE_BUILD_TYPE=RELEASE
                      -DBUILD_THIRDPARTY=ON
                      -DBUILD_STATIC_LIBS=ON
                      ..
    BUILD_COMMAND cd build && make
    INSTALL_COMMAND cd build && make install
)

ExternalProject_Add(
    libjxl
    URL https://github.com/libjxl/libjxl/archive/refs/tags/v0.10.3.tar.gz
    URL_MD5 0fd3db8956a41d13b5e8eac4fe61d8d3
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND mkdir -p build && cd build &&
                    sed -i "/download_github testdata/d" <SOURCE_DIR>/deps.sh &&
                    sed -i "/download_github third_party\\/libpng/d" <SOURCE_DIR>/deps.sh &&
                    sed -i "/download_github third_party\\/zlib/d" <SOURCE_DIR>/deps.sh &&
                    sed -i "/download_github third_party\\/sjpeg/d" <SOURCE_DIR>/deps.sh &&
                    sed -i "/download_github third_party\\/libjpeg-turbo/d" <SOURCE_DIR>/deps.sh &&
                    <SOURCE_DIR>/deps.sh &&
                    cmake
                    -DCMAKE_CXX_FLAGS="-I${STATIC_LIBS_DIR}/include"
                    -DCMAKE_EXE_LINKER_FLAGS="-L${STATIC_LIBS_DIR}/lib -L${STATIC_LIBS_DIR}/lib64"
                    -DCMAKE_INCLUDE_PATH=${STATIC_LIBS_DIR}/include
                    -DCMAKE_LIBRARY_PATH=${STATIC_LIBS_DIR}/lib
                    -DCMAKE_INSTALL_PREFIX=${STATIC_LIBS_DIR}
                    -DCMAKE_BUILD_TYPE=Release
                    -DBUILD_TESTING=OFF
                    -DBUILD_SHARED_LIBS=OFF
                    -DJPEGXL_INSTALL_JPEGLI_LIBJPEG=OFF
                    -DJPEGXL_ENABLE_DOXYGEN=OFF
                    -DJPEGXL_ENABLE_MANPAGES=OFF
                    -DJPEGXL_ENABLE_BENCHMARK=OFF
                    -DJPEGXL_ENABLE_EXAMPLES=OFF
                    -DJPEGXL_ENABLE_JNI=OFF
                    -DJPEGXL_ENABLE_SJPEG=OFF
                    -DJPEGXL_BUNDLE_LIBPNG=OFF
                    -DJPEGXL_ENABLE_TOOLS=OFF
                    -DJPEGXL_ENABLE_JPEGLI=OFF
                    ..
    BUILD_COMMAND cd build && cmake --build . -- -j1
    INSTALL_COMMAND cd build && make install
)
