
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
    imlib2
    URL https://sourceforge.net/projects/enlightenment/files/imlib2-src/1.12.2/imlib2-1.12.2.tar.gz/download
    URL_MD5 15e080e450b8bf3709292cc3efc986e3
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
    BUILD_COMMAND ${CMAKE_COMMAND} -E make_directory ${IMLIB2_EXTERN} && 
                    ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/src/modules/loaders ${IMLIB2_EXTERN}/loaders && 
                    ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/src/lib/Imlib2_Loader.h ${IMLIB2_EXTERN} && 
                    ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/config.h ${IMLIB2_EXTERN}/loaders
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)


set(IMYLIB2 imylib2)

project(${IMYLIB2}
    VERSION 1.0 
    LANGUAGES C
)

set(IMYLIB2_SOURCES 
    ./imylib2.c
    ./imylib2.h
    ./imlib2/Imlib2_Loader.h
    ./imlib2/loaders/exif.c

    ./loaders/loader_qoi.c
    ./loaders/loader_bmp.c
)


find_library(JPEG_FOUND jpeg)

if(LIBJPEG_FOUND)
    message(STATUS "Found mylib: ${LIBJPEG_FOUND}")
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_jpeg.c)
endif()

find_library(OPENJPEG_FOUND openjpeg)

if(OPENJPEG_FOUND)
    message(STATUS "Found mylib: ${OPENJPEG_FOUND}")
    list(APPEND IMYLIB2_SOURCES ./loaders/loader_j2k.c)
endif()


add_library(${IMYLIB2} STATIC ${IMYLIB2_SOURCES})

target_include_directories(${IMYLIB2} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



