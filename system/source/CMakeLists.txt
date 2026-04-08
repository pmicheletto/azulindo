set(AZULINDO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(AZULINDO_INCLUDE_DIRS
    ${AZULINDO_SOURCE_DIR}
    ${AZULINDO_SOURCE_DIR}/..
    ${AZULINDO_SOURCE_DIR}/../raylib/src
    ${AZULINDO_SOURCE_DIR}/../../llama.cpp/include
    ${AZULINDO_SOURCE_DIR}/../../llama.cpp/src
)

add_library(azulindo_core STATIC
    core/brain/brain.cpp
    core/state_emotion/emotion_profile.cpp
    core/state_emotion/emotion_manager.cpp
)

set_target_properties(azulindo_core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(azulindo_core PUBLIC
    ${AZULINDO_INCLUDE_DIRS}
)

target_link_libraries(azulindo_core PUBLIC
    llama
    m
    pthread
    dl
    rt
)

add_library(azulindo_ui STATIC
    screen/azulindo_screen.cpp
    screen/hologram.cpp
)

set_target_properties(azulindo_ui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(azulindo_ui PUBLIC
    ${AZULINDO_INCLUDE_DIRS}
    ${AZULINDO_SOURCE_DIR}/screen
)

target_link_libraries(azulindo_ui PUBLIC
    azulindo_core
    raylib
)

add_executable(azulindo_app
    main.cpp
    app/azulindo_application.cpp
)

set_target_properties(azulindo_app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(azulindo_app PRIVATE
    azulindo_ui
)

if(NOT APPLE AND NOT WIN32)
    target_link_libraries(azulindo_app PRIVATE GLESv2 EGL drm gbm)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
