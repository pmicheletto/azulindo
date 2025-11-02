find_package(SDL3 REQUIRED)

add_executable(source "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET source PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(source PRIVATE SDL3::SDL3)
