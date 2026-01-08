add_executable(azulindo_app "main.cpp")

set_target_properties(azulindo_app PROPERTIES CXX_STANDARD 20)

target_include_directories(azulindo_app PRIVATE "${CMAKE_SOURCE_DIR}/raylib/src")

target_link_libraries(azulindo_app PRIVATE 
    raylib 
    GL 
    m 
    pthread 
    dl 
    rt 
    X11
)