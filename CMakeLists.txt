cmake_minimum_required(VERSION 3.1)

project(SFMLTemplate)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Setup executable
add_executable(SFMLTemplate
	"src/main.cpp"
	"src/Application.cpp"
	"src/Application.hpp"

	"src/Framework/Framework.cpp"
	"src/Framework/Framework.hpp"
	"src/Framework/Graphics.cpp"
	"src/Framework/Graphics.hpp"
	"src/Framework/Utility.cpp"
	"src/Framework/Utility.hpp"
)

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

## Setup TinyOBJLoader
set(TOL_DIR "${PROJECT_SOURCE_DIR}/extern/tinyobjloader")
add_library(tol STATIC "${TOL_DIR}/tiny_obj_loader.cc" "${TOL_DIR}/tiny_obj_loader.h")
target_include_directories(tol PRIVATE "${TOL_DIR}")
target_include_directories(SFMLTemplate PRIVATE "${TOL_DIR}")

## Setup GLM
target_include_directories(SFMLTemplate PRIVATE "${PROJECT_SOURCE_DIR}/extern/glm-0.9.9.8/glm")

## Link dependencies
target_link_libraries(SFMLTemplate sfml-graphics sfml-audio glad tol)

## Copy dependencies DLLs
add_custom_command(TARGET SFMLTemplate POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/extern/SFML-2.5.1-windows-vc15-64-bit/SFML-2.5.1/bin" "$<TARGET_FILE_DIR:SFMLTemplate>")

## Copy data folder
add_custom_command(TARGET SFMLTemplate POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:SFMLTemplate>/data")
