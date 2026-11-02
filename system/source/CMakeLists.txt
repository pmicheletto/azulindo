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
    screen/ai_text_pipeline.cpp
    screen/dialogue_panel.cpp
    screen/emotion_animator.cpp
    screen/hologram.cpp
    screen/hud_panel.cpp
    screen/text_layout.cpp
    screen/wave_renderer.cpp
)

set_target_properties(azulindo_ui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(azulindo_ui PUBLIC
    ${AZULINDO_INCLUDE_DIRS}
)

# Emotion profiles live in core; keep llama out of the public UI link surface.
target_link_libraries(azulindo_ui
    PUBLIC raylib
    PRIVATE azulindo_core
)

add_executable(azulindo_app
    main.cpp
    app/azulindo_application.cpp
    app/emotion_hotkeys.cpp
    app/language_selector.cpp
    app/prompt_controller.cpp
    app/status_overlay.cpp
    app/token_stream_controller.cpp
    app/utf8_input.cpp
)

set_target_properties(azulindo_app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(azulindo_app PRIVATE
    azulindo_ui
    azulindo_core
)

if(PLATFORM STREQUAL "DRM")
    target_compile_definitions(azulindo_app PRIVATE AZULINDO_PLATFORM_DRM)
endif()

set_target_properties(azulindo_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
