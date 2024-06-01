

cmake_minimum_required(VERSION 3.7...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()


project(doko 
    VERSION 1.0 
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# this folder cotains the a CMakeLists.txt for raylib
set(RAYLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/raylib/raylib-5.0/)
set(RAYGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/raylib/raygui-4.0/)

# the bundler will generate this file
set(BUNDLER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bundle.h)


# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_VERBOSE_MAKEFILE ON)
# endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PACKAGE_RESOURCES ON)
else()
    set(PACKAGE_RESOURCES OFF)
endif()

message(STATUS "CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Packaging Resources: ${PACKAGE_RESOURCES}")


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
    ${BUNDLER_OUTPUT}
)

if(PACKAGE_RESOURCES)

    message(STATUS "Will compile resources")

    # define the bundler
    add_executable(bundler src/bundler.c)
    
    # command to run the bundler
    add_custom_command(
        OUTPUT ${BUNDLER_OUTPUT}
        COMMAND bundler
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating header file with compiled resources"
        DEPENDS bundler
    )

    # Add a custom target that depends on the generated header file
    add_custom_target(run_bundle ALL
        DEPENDS ${BUNDLER_OUTPUT}
    )

    # define the macro which says to use the bundle header
    target_compile_definitions(${PROJECT_NAME} PRIVATE DOKO_BUNDLE)

else()

    # ensure cmake will rebuild without this having resources
    file(WRITE ${BUNDLER_OUTPUT} "")

    # fake command that does nothing
    add_custom_target(run_bundle ALL COMMAND "")
endif()

# optionally run the bundler
add_dependencies(${PROJECT_NAME} run_bundle)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
target_include_directories(${PROJECT_NAME} PUBLIC "${RAYLIB_ROOT}/src")
target_include_directories(${PROJECT_NAME} PUBLIC "${RAYGUI_ROOT}/src")

#########################
# Config & Build Raylib #
#########################

# ensure raylib uses our options
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)

# only for desktop
set(PLATFORM Desktop CACHE STRING "" FORCE)

# stuff we are not using
set(USE_AUDIO               OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES          OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES             OFF CACHE BOOL "" FORCE)
set(SUPPORT_CAMERA_SYSTEM   OFF CACHE BOOL "" FORCE)
set(SUPPORT_SCREEN_CAPTURE  OFF CACHE BOOL "" FORCE)
set(SUPPORT_GIF_RECORDING   OFF CACHE BOOL "" FORCE)
set(SUPPORT_GESTURES_SYSTEM OFF CACHE BOOL "" FORCE)
set(SUPPORT_MOUSE_GESTURES  OFF CACHE BOOL "" FORCE)
set(SUPPORT_VR_SIMULATOR    OFF CACHE BOOL "" FORCE)

# support all the image formats
set(SUPPORT_FILEFORMAT_PNG     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_BMP     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_TGA     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_GIF     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_QOI     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PSD     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_DDS     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_HDR     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PIC     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_KTX     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_ASTC    ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PKM     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PVR     ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_SVG     ON CACHE BOOL "" FORCE)

set(SUPPORT_IMAGE_GENERATION   ON CACHE BOOL "" FORCE)
set(SUPPORT_IMAGE_MANIPULATION ON CACHE BOOL "" FORCE)

# we need this to write png data in memory to clipboard
set(SUPPORT_SAVE_PNG ON CACHE BOOL "" FORCE)

# run the raylib CMakeLists.txt
add_subdirectory(${RAYLIB_ROOT})



#########################
# Checking for Imlib2   #
#########################

find_library(IMLIB Imlib2)

if(IMLIB)
    message(STATUS "Imlib2 found")

    # add imlib2
    target_link_libraries(${PROJECT_NAME} PUBLIC Imlib2)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DOKO_USE_IMLIB2)

else()
    message(STATUS "Imlib2 not found, proceeding without it")
endif()

 
