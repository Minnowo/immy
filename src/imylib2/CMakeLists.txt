
cmake_minimum_required(VERSION 3.7...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# We are going to determine the c std
# and we are going to use the latest
get_property(c_features GLOBAL PROPERTY CMAKE_C_KNOWN_FEATURES)

# c_std_90;c_std_99;c_std_11;c_std_17;c_std_23
if("c_std_23" IN_LIST c_features)
    set(CMAKE_C_STANDARD 23)
elseif("c_std_17" IN_LIST c_features)
    set(CMAKE_C_STANDARD 17)
elseif("c_std_11" IN_LIST c_features)
    set(CMAKE_C_STANDARD 11)
elseif("c_std_99" IN_LIST c_features)
    set(CMAKE_C_STANDARD 99)
elseif("c_std_90" IN_LIST c_features)
    set(CMAKE_C_STANDARD 90)
endif()

message(STATUS "C_STANDARD is: ${CMAKE_C_STANDARD}")

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PACKAGE_RESOURCES ON)
else()
    set(PACKAGE_RESOURCES OFF)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

message(STATUS "CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Packaging Resources: ${PACKAGE_RESOURCES}")


# Where the external imlib2 code will go
set(IMLIB2_EXTERN ${CMAKE_CURRENT_SOURCE_DIR}/imlib2/)
message(STATUS "Imlib2 should be in ${IMLIB2_EXTERN}")


#
# Downloads Imlib2 v1.12.2 and takes the loader files we want
#
include(ExternalProject)
ExternalProject_Add(
    imlib2_loaders
    URL https://sourceforge.net/projects/enlightenment/files/imlib2-src/1.12.2/imlib2-1.12.2.tar.gz/download
    URL_MD5 15e080e450b8bf3709292cc3efc986e3
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --disable-text --without-x
    BUILD_COMMAND ${CMAKE_COMMAND} -E make_directory ${IMLIB2_EXTERN} &&
                    ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/src/modules/loaders ${IMLIB2_EXTERN}/loaders &&
                    ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/src/lib/Imlib2_Loader.h ${IMLIB2_EXTERN} &&
                    ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/config.h ${IMLIB2_EXTERN}/loaders
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)

add_custom_command(
    OUTPUT ${IMLIB2_EXTERN}/Imlib2_Loader.h
           ${IMLIB2_EXTERN}/loaders/exif.c
    DEPENDS imlib2_loaders
    COMMAND ${CMAKE_COMMAND} -E echo "Generated Imlib2_Loader.h"
)

add_custom_target(ensure_imlib2_loaders
    DEPENDS ${IMLIB2_EXTERN}/Imlib2_Loader.h
            ${IMLIB2_EXTERN}/loaders/exif.c
)


set(IMYLIB2 imylib2)

project(${IMYLIB2}
    VERSION 1.0
    LANGUAGES C
)

set(DEFINITIONS )
set(LINK_LIBS )
set(IMYLIB2_SOURCES
    ./imylib2.c
    ./imylib2.h
    ./imlib2/Imlib2_Loader.h
    ./imlib2/loaders/exif.c

    ./loaders/loader_qoi.c
    ./loaders/loader_bmp.c
    ./loaders/loader_ani.c
    ./loaders/loader_argb.c
    ./loaders/loader_ff.c
    ./loaders/loader_ico.c
    ./loaders/loader_lbm.c
    ./loaders/loader_pnm.c
    ./loaders/loader_tga.c
    ./loaders/loader_xbm.c
    # ./loaders/loader_xpm.c
)


# testing with having all the image libraries built from source
set(BUILD_EVERYTHING_STATIC OFF)

if(NOT BUILD_EVERYTHING_STATIC)
    set(CUSTOM_INCLUDE_PATH /usr)
    set(CUSTOM_LIBS_PATH /usr)

    set(LIBS_SEARCH
        ${CUSTOM_LIBS_PATH}/lib
        ${CUSTOM_LIBS_PATH}/lib64
    )

    set(INCLUDE_SEARCH
        ${CUSTOM_INCLUDE_PATH}/include
        ${CUSTOM_INCLUDE_PATH}/include/openjpeg-2.5
        ${CUSTOM_INCLUDE_PATH}/include/openjpeg-2.4
    )
else()
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

    include(BuildStatic)

    set(CUSTOM_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}/static_libs)
    set(CUSTOM_LIBS_PATH ${CMAKE_CURRENT_BINARY_DIR}/static_libs)

    set(LIBS_SEARCH
        ${CUSTOM_LIBS_PATH}/lib
        ${CUSTOM_LIBS_PATH}/lib64
        NO_DEFAULT_PATH
    )

    set(INCLUDE_SEARCH
        ${CUSTOM_INCLUDE_PATH}/include
        ${CUSTOM_INCLUDE_PATH}/include/openjpeg-2.5
        ${CUSTOM_INCLUDE_PATH}/include/openjpeg-2.4
        NO_DEFAULT_PATH
    )
endif()

# prefer static over shared
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

find_library(JPEG_FOUND jpeg PATHS ${LIBS_SEARCH} )
find_path(JPEG_INCLUDE jpeglib.h PATHS ${INCLUDE_SEARCH})

if(JPEG_FOUND AND JPEG_INCLUDE)

    message(STATUS "Found library: ${JPEG_FOUND}")
    message(STATUS "Including: ${JPEG_INCLUDE}")

    list(APPEND LINK_LIBS ${JPEG_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_jpeg.c)
    list(APPEND DEFINITIONS BUILD_JPEG_LOADER)

    include_directories(${JPEG_INCLUDE})
endif()


find_library(OPENJPEG_FOUND openjp2 PATHS ${LIBS_SEARCH})
find_path(OPENJPEG_INCLUDE openjpeg.h PATHS ${INCLUDE_SEARCH})

if(OPENJPEG_FOUND AND OPENJPEG_INCLUDE)

    message(STATUS "Found library: ${OPENJPEG_FOUND}")
    message(STATUS "Including: ${OPENJPEG_INCLUDE}")

    list(APPEND LINK_LIBS ${OPENJPEG_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_j2k.c)
    list(APPEND DEFINITIONS BUILD_JP2_LOADER)

    include_directories(${OPENJPEG_INCLUDE})
endif()


find_library(JXL_FOUND jxl PATHS ${LIBS_SEARCH})
find_library(JXLTHREADS_FOUND jxl_threads PATHS ${LIBS_SEARCH})
find_path(JXL_INCLUDE
    jxl/encode.h
    jxl/decode.h
    jxl/thread_parallel_runner.h
    PATHS ${INCLUDE_SEARCH})

if(JXL_FOUND AND JXLTHREADS_FOUND AND JXL_INCLUDE)

    message(STATUS "Found library: ${JXL_FOUND}")
    message(STATUS "Found library: ${JXLTHREADS_FOUND}")
    message(STATUS "Including: ${JXL_INCLUDE}")

    list(APPEND LINK_LIBS ${JXL_FOUND})
    list(APPEND LINK_LIBS ${JXLTHREADS_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_jxl.c)
    list(APPEND DEFINITIONS BUILD_JXL_LOADER)

    include_directories(${JXL_INCLUDE})
endif()


find_library(HEIF_FOUND heif PATHS ${LIBS_SEARCH})
find_path(HEIF_INCLUDE
    libheif/heif.h
    PATHS ${INCLUDE_SEARCH})

if(HEIF_FOUND AND HEIF_INCLUDE)

    message(STATUS "Found library: ${HEIF_FOUND}")
    message(STATUS "Including: ${HEIF_INCLUDE}")

    list(APPEND LINK_LIBS ${HEIF_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_heif.c)
    list(APPEND DEFINITIONS BUILD_HEIF_LOADER)

    include_directories(${HEIF_INCLUDE})
endif()


find_library(PNG_FOUND png16 PATHS ${LIBS_SEARCH})
find_path(PNG_INCLUDE
    libpng16/png.h
    PATHS ${INCLUDE_SEARCH})

if(PNG_FOUND AND PNG_INCLUDE)

    message(STATUS "Found library: ${PNG_FOUND}")
    message(STATUS "Including: ${PNG_INCLUDE}")

    list(APPEND LINK_LIBS ${PNG_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_png.c)
    list(APPEND DEFINITIONS BUILD_PNG_LOADER)

    include_directories(${PNG_INCLUDE})
endif()



find_library(TIFF_FOUND tiff PATHS ${LIBS_SEARCH})
find_path(TIFF_INCLUDE
    tiffio.h
    PATHS ${INCLUDE_SEARCH})

if(TIFF_FOUND AND TIFF_INCLUDE)

    message(STATUS "Found library: ${TIFF_FOUND}")
    message(STATUS "Including: ${TIFF_INCLUDE}")

    list(APPEND LINK_LIBS ${TIFF_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_tiff.c)
    list(APPEND DEFINITIONS BUILD_TIFF_LOADER)

    include_directories(${TIFF_INCLUDE})
endif()



find_library(WEBP_FOUND webp PATHS ${LIBS_SEARCH})
find_library(WEBPDEMUX_FOUND webpdemux PATHS ${LIBS_SEARCH})
find_library(SHARPYUV_FOUND sharpyuv PATHS ${LIBS_SEARCH})
find_path(WEBP_INCLUDE
    webp/decode.h
    webp/encode.h
    webp/dmux.h
    PATHS ${INCLUDE_SEARCH})

if(WEBP_FOUND AND WEBPDEMUX_FOUND AND SHARPYUV_FOUND AND WEBP_INCLUDE)

    message(STATUS "Found library: ${WEBP_FOUND}")
    message(STATUS "Found library: ${WEBPDEMUX_FOUND}")
    message(STATUS "Found library: ${SHARPYUV_FOUND}")
    message(STATUS "Including: ${WEBP_INCLUDE}")

    list(APPEND LINK_LIBS ${WEBP_FOUND})
    list(APPEND LINK_LIBS ${WEBPDEMUX_FOUND})
    list(APPEND LINK_LIBS ${SHARPYUV_FOUND})
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_webp.c)
    list(APPEND DEFINITIONS BUILD_WEBP_LOADER)

    include_directories(${WEBP_INCLUDE})
endif()


add_library(${IMYLIB2} STATIC ${IMYLIB2_SOURCES})

add_dependencies(${IMYLIB2} imlib2_loaders)
add_dependencies(${IMYLIB2} ensure_imlib2_loaders)

target_include_directories(${IMYLIB2} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${IMYLIB2} PUBLIC ${IMLIB2_EXTERN})
target_link_libraries(${IMYLIB2} ${LINK_LIBS})
target_compile_definitions(${IMYLIB2} PUBLIC ${DEFINITIONS})



# # For testing
# add_executable(imylib2_test ./main.c)
# target_link_libraries(imylib2_test ${IMYLIB2})
# target_compile_definitions(imylib2_test PUBLIC ${DEFINITIONS})

