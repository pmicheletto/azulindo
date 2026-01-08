cmake_minimum_required(VERSION 3.14)
project(AzulindoIA)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

# Disable raylib examples and tests
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory("raylib")

add_subdirectory("source")