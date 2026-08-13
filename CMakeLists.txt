cmake_minimum_required(VERSION 3.5)
project(game C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Remove -rdynamic if it exists
    string(REPLACE "-rdynamic" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
    string(REPLACE "-rdynamic" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

    # Point to Windows Raylib includes and libs - update these paths accordingly!
    include_directories("/home/benja/raylib/src")
    link_directories("/home/benja/raylib/build-windows/raylib")
    set(PLATFORM_LIBS raylib)

    set(PLATFORM_LIBS
        raylib
        winmm
        gdi32
        opengl32
        kernel32
        user32
    )
else()
    # Linux includes and libs
    include_directories("/home/benja/raylib/src")
    link_directories("/home/benja/raylib/src")

    set(PLATFORM_LIBS
        raylib
        m
        pthread
        dl
        rt
        X11
    )
endif()

# Define source files
file(GLOB SBLIB_SRC "sblib/src/*.c")
set(SBTILES_SRC "sbTiles.c")
# Create executable
add_executable(${PROJECT_NAME}  ${SBLIB_SRC} ${SBTILES_SRC})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
