cmake_minimum_required(VERSION 3.15)
project(engine3d VERSION 1.0.0 LANGUAGES C CXX)

option(ENABLE_ASAN "Enable Address Sanitizer" ON)
option(BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(glfw3 3.3 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(assimp REQUIRED)

if(ENABLE_ASAN)
    message(STATUS "Address Sanitizer (ASAN) is ENABLED")
    add_compile_options(-fsanitize=address -g -O0)
    add_link_options(-fsanitize=address)
else()
    message(STATUS "Address Sanitizer (ASAN) is DISABLED")
endif()

add_compile_options(-Wall -Wextra -Wuninitialized -Wmaybe-uninitialized)

set(SOURCES 
    src/bufferlayout.cpp
    src/gl.c 
    src/stb_image.c
    src/camera.cpp
    src/framebuffer.cpp
    src/gfxengine.cpp
    src/material.cpp
    src/mesh.cpp
    src/model.cpp
    src/shader.cpp
    src/texture.cpp
    src/util.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    assimp::assimp
)

if(BUILD_EXAMPLES)
    add_executable(engine3d_example
	examples/main.cpp
    )

    target_include_directories(engine3d_example
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/external
    )

    target_link_libraries(engine3d_example
	PRIVATE
	glfw
	${PROJECT_NAME}
    )
endif()
