cmake_minimum_required(VERSION 3.15)
project(MyGLEngine VERSION 1.0.0 LANGUAGES C CXX)

# --- Options ---
# Easily toggle ASAN on/off here. Default is ON since you are debugging.
option(ENABLE_ASAN "Enable Address Sanitizer" ON)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
# 1. GLFW
find_package(glfw3 3.3 REQUIRED)

# 2. OpenGL
find_package(OpenGL REQUIRED)

# 3. Assimp
# On Arch/Linux, this usually provides the 'assimp::assimp' target
find_package(assimp REQUIRED)

# --- Compiler Flags ---
if(ENABLE_ASAN)
    message(STATUS "Address Sanitizer (ASAN) is ENABLED")
    add_compile_options(-fsanitize=address -g -O0)
    add_link_options(-fsanitize=address)
else()
    message(STATUS "Address Sanitizer (ASAN) is DISABLED")
endif()

# Warnings (Optional but recommended)
add_compile_options(-Wall -Wextra -Wuninitialized -Wmaybe-uninitialized)

# --- Source Files ---
# Add all your .cpp files here. 
# CRITICAL: You must include glad.c here!
set(SOURCES 
    src/glad.c 
    src/stb_image.c
    src/camera.cpp
    src/gfxengine.cpp
    src/main.cpp
    src/mesh.cpp
    src/model.cpp
    src/object.cpp
    src/shader.cpp
    src/texture.cpp
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_glfw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Include Directories ---
# Tells CMake where to find header files (like glad/glad.h)
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
)

# --- Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
    assimp::assimp
    dl              # Required for GLAD to load dynamic libraries on Linux
    pthread         # Often required by dependencies
)
