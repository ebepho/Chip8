
cmake_minimum_required(VERSION 3.15)
project(Chip8Emulator CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pkg-config and SDL2/SDL2_image packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2       REQUIRED sdl2)
pkg_check_modules(SDL2IMAGE  REQUIRED SDL2_image)

# Add ImGui
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

# Create ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Add ImGui include directories
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${SDL2_INCLUDE_DIRS}
)

# Link SDL2 to ImGui
target_link_libraries(imgui PRIVATE ${SDL2_LIBRARIES})

# Define the main executable and source files
add_executable(chip8
    src/main.cpp
    src/chip8.cpp
    src/instructions.cpp
    UI/graphics.cpp
)

# Add project header directories
target_include_directories(chip8 PRIVATE include UI ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

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

# Link libraries
target_link_libraries(chip8 PRIVATE
    ${SDL2IMAGE_LIBRARIES}
    imgui
)

# Add any extra compile flags from pkg-config (not link flags)
target_compile_options(chip8 PRIVATE
    ${SDL2_CFLAGS_OTHER}
    ${SDL2IMAGE_CFLAGS_OTHER}
)
