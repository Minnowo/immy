

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
    # set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

message(STATUS "CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Packaging Resources: ${PACKAGE_RESOURCES}")

# The SOURCE_PATH_SIZE is used in the c code
# to remove the absolute path of the __FILE__ macro
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# source folders
set(SRC_ROOT     ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(RAYLIB_ROOT  ${SRC_ROOT}/raylib/raylib-5.0)
set(IMYLIB2_ROOT ${SRC_ROOT}/imylib2)
set(IMMY_ROOT    ${SRC_ROOT}/immy)

# the bundler will generate this file
set(BUNDLER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bundle.h)


project(immy
    VERSION 1.0 
    LANGUAGES C
)


add_executable(${PROJECT_NAME} 
    ${IMMY_ROOT}/config.h
    ${IMMY_ROOT}/main.c
    ${IMMY_ROOT}/implementations.c
    ${IMMY_ROOT}/core/core.h
    ${IMMY_ROOT}/core/core.c
    ${IMMY_ROOT}/core/image_async.c
    ${IMMY_ROOT}/core/image.c
    ${IMMY_ROOT}/core/str.c
    ${IMMY_ROOT}/core/tostring.c
    ${IMMY_ROOT}/core/ffmpeg.c
    ${IMMY_ROOT}/core/imagemagick.c
    ${IMMY_ROOT}/core/imlib.c
    ${IMMY_ROOT}/core/clipboard.c
    ${IMMY_ROOT}/core/win32.c
    ${IMMY_ROOT}/core/unix.c
    ${IMMY_ROOT}/core/x11.c
    ${IMMY_ROOT}/input.c
    ${IMMY_ROOT}/input.h
    ${IMMY_ROOT}/resources.c
    ${IMMY_ROOT}/resources.h
    ${IMMY_ROOT}/ui/ui.c
    ${IMMY_ROOT}/ui/ui.h
    ${IMMY_ROOT}/ui/screen/files.c
    ${IMMY_ROOT}/ui/screen/image.c
    ${IMMY_ROOT}/ui/screen/keybinds.c
    ${IMMY_ROOT}/ui/screen/thumbs.c
    ${IMMY_ROOT}/external/sha256.c
    ${IMMY_ROOT}/external/sha256.h
    ${IMMY_ROOT}/external/hashmap.c
    ${IMMY_ROOT}/external/hashmap.h
    ${IMMY_ROOT}/external/strnatcmp.c
    ${IMMY_ROOT}/external/strnatcmp.h
    ${IMMY_ROOT}/external/miniz.h
    ${IMMY_ROOT}/external/miniz.c

    ${BUNDLER_OUTPUT}
)

if(PACKAGE_RESOURCES)

    message(STATUS "Will compile resources")

    # define the bundler
    add_executable(bundler ${SRC_ROOT}/bundler.c)
    
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
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_RESOURCE_BUNDLE)

else()

    # ensure cmake will rebuild without this having resources
    file(WRITE ${BUNDLER_OUTPUT} "")

    # fake command that does nothing
    add_custom_target(run_bundle ALL COMMAND "")
endif()

# optionally run the bundler
add_dependencies(${PROJECT_NAME} run_bundle)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

##########################
# Config & Build Imylib2 #
##########################
if(UNIX)
    message(STATUS "We are UNIX. imylib2 can be used.")
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMYLIB2_AVAILABLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC imylib2)
    target_include_directories(${PROJECT_NAME} PUBLIC "${IMYLIB2_ROOT}")
    add_subdirectory(${IMYLIB2_ROOT})
endif()


#########################
# Config & Build Raylib #
#########################

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
target_include_directories(${PROJECT_NAME} PUBLIC "${RAYLIB_ROOT}/src")

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

find_library(IMLIB_FOUND Imlib2)

if(IMLIB_FOUND)
    message(STATUS "Imlib2 found")

    # add imlib2
    target_link_libraries(${PROJECT_NAME} PUBLIC Imlib2)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IMLIB_AVAILABLE)

else()
    message(STATUS "Imlib2 not found, proceeding without it")
endif()


#########################
# Checking for X11      #
#########################
find_library(X11_FOUND X11)

if(X11_FOUND)

    message(STATUS "X11 found")

    target_link_libraries(${PROJECT_NAME} PUBLIC X11)
    target_compile_definitions(${PROJECT_NAME} PRIVATE X11_AVAILABLE)

else()
    message(WARNING "X11 not found.")
endif()

