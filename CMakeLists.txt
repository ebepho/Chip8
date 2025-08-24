
cmake_minimum_required(VERSION 3.15)
project(Chip8Emulator CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pkg-config and SDL2/SDL2_image packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2       REQUIRED sdl2)
pkg_check_modules(SDL2IMAGE  REQUIRED SDL2_image)

# Define the main executable and source files
add_executable(chip8
    src/main.cpp
    src/chip8.cpp
    src/instructions.cpp
    UI/graphics.cpp
)

# Add project header directories
target_include_directories(chip8 PRIVATE include UI)

# Add SDL2 and SDL2_image include directories from pkg-config
target_include_directories(chip8 PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
)

# Add library search paths for SDL2 and SDL2_image
target_link_directories(chip8 PRIVATE
    ${SDL2_LIBRARY_DIRS}
    ${SDL2IMAGE_LIBRARY_DIRS}
)

# Link only SDL2_image (it pulls in SDL2 automatically)
target_link_libraries(chip8 PRIVATE
    ${SDL2IMAGE_LIBRARIES}
)

# Add any extra compile flags from pkg-config (not link flags)
target_compile_options(chip8 PRIVATE
    ${SDL2_CFLAGS_OTHER}
    ${SDL2IMAGE_CFLAGS_OTHER}
)
