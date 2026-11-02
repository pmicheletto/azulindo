cmake_minimum_required(VERSION 3.25)

project(azulindo_interface LANGUAGES CXX)

set(LLAMA_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(LLAMA_BUILD_TESTS OFF CACHE INTERNAL "")
set(LLAMA_SERVER OFF CACHE INTERNAL "")

add_subdirectory(../llama.cpp ${CMAKE_CURRENT_BINARY_DIR}/llama_build)
add_subdirectory(raylib)
add_subdirectory(source)
