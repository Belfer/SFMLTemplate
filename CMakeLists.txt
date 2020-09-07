cmake_minimum_required(VERSION 3.1)

project(SFMLTemplate)

## Setup executable
add_executable(SFMLTemplate src/main.cpp src/Renderer.cpp src/Renderer.hpp)
target_include_directories(SFMLTemplate PRIVATE "${PROJECT_SOURCE_DIR}/src")

## Setup SFML
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "${PROJECT_SOURCE_DIR}/extern/SFML-2.5.1-windows-vc15-64-bit/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

## Setup GLAD
set(GLAD_DIR "${PROJECT_SOURCE_DIR}/extern/glad")
add_library(glad STATIC "${GLAD_DIR}/src/glad.c" "${GLAD_DIR}/include/glad/glad.h" "${GLAD_DIR}/include/KHR/khrplatform.h")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")
target_include_directories(SFMLTemplate PRIVATE "${GLAD_DIR}/include")

## Setup GLM
target_include_directories(SFMLTemplate PRIVATE "${PROJECT_SOURCE_DIR}/extern/glm-0.9.9.8/glm")

## Link dependencies
target_link_libraries(SFMLTemplate sfml-graphics sfml-audio glad)

add_custom_command(TARGET SFMLTemplate POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/extern/SFML-2.5.1-windows-vc15-64-bit/SFML-2.5.1/bin" $<TARGET_FILE_DIR:SFMLTemplate>)