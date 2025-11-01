cmake_minimum_required(VERSION 3.22)
project(DAW3 VERSION 0.0.1)
find_package(JUCE CONFIG REQUIRED)
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE")  
juce_add_console_app(DAW3 daw3 "daw3 as it is")
juce_generate_juce_header(DAW3)

target_sources(DAW3
    PRIVATE
        src/main.cpp
)

target_compile_definitions(DAW3
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(DAW3
    PRIVATE
        juce::juce_core
        juce::juce_audio_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(DAW3
    PRIVATE
        ${JUCE_DIR}
        ${JUCE_DIR}/modules
)
