cmake_minimum_required(VERSION 3.22)
project(DAW3 VERSION 0.0.1)

set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE")
add_subdirectory(${JUCE_DIR})

add_executable(DAW3
    src/main.cpp
    src/daw3/daw3.cpp
    src/audio_engine/audio_engine.cpp
    src/audio_engine/instrumental_track/instrumental_track.cpp
    src/audio_engine/track_handler/track_handler.cpp
    src/ui/main_window/main_window.cpp
    src/ui/ui.cpp
)

target_compile_definitions(DAW3
    PRIVATE
        JUCE_USE_CUSTOM_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_PLUGINHOST_VST3=1 # only VST3
        JUCE_PLUGINHOST_VST=0  # no VST2 
)

target_link_libraries(DAW3
    PRIVATE
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(DAW3
    PRIVATE
        ${JUCE_DIR}
        ${JUCE_DIR}/modules
)

target_compile_features(DAW3 PRIVATE cxx_std_17)
