

cmake_minimum_required(VERSION 3.7...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_C_STANDARD 23)
# set(CMAKE_BUILD_TYPE "Release")

# this folder cotains the a CMakeLists.txt for raylib
set(RAYLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/raylib/raylib-5.0/)


# my stuff
project(doko 
    VERSION 1.0 
    LANGUAGES C
)

add_executable(${PROJECT_NAME} 
    src/main.c
    src/doko.c
    src/doko.h
    src/doko_async.c
    src/input.c
    src/input.h
    src/ui.c
    src/ui.h
    src/resources.c
    src/resources.h
    src/external/hashmap.c
    src/external/hashmap.h
    src/external/strnatcmp.c
    src/external/strnatcmp.h
)

target_compile_definitions(doko PRIVATE DOKO_BUNDLE)

# make sure we have raylib
add_subdirectory(${RAYLIB_ROOT})
target_link_libraries(doko PUBLIC raylib)
target_include_directories(doko PUBLIC "${RAYLIB_ROOT}/src")

# add imlib2
target_link_libraries(doko PUBLIC Imlib2)

