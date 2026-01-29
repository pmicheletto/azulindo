add_executable(azulindo_app
    main.cpp
    screen/azulindo_screen.cpp
    core/emotion_profile.cpp
    screen/hologram.cpp
)

set_target_properties(azulindo_app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(azulindo_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}../
    ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src 
)

target_link_libraries(azulindo_app PRIVATE
    raylib
    GL
    m
    pthread
    dl
    rt
    X11
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
