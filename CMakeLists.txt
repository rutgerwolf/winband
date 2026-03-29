cmake_minimum_required(VERSION 3.22)
project(WinBand VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE (git submodule: third_party/JUCE) ──────────────────────────────────
add_subdirectory(third_party/JUCE)

# ── Aubio (git submodule: third_party/aubio) ────────────────────────────────
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/aubio/CMakeLists.txt")
    add_subdirectory(third_party/aubio EXCLUDE_FROM_ALL)
    set(AUBIO_LIBRARIES aubio)
    set(AUBIO_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/aubio/src")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(AUBIO REQUIRED aubio)
endif()

# ── Application target ──────────────────────────────────────────────────────
juce_add_gui_app(WinBand
    PRODUCT_NAME     "WinBand"
    COMPANY_NAME     "WinBand Project"
    VERSION          "0.1.0"
    NEEDS_CURL        FALSE
    NEEDS_WEB_BROWSER FALSE
)

target_sources(WinBand
    PRIVATE
        src/main.cpp
        src/MainComponent.cpp
        src/audio/AudioEngine.cpp
        src/audio/AsioManager.cpp
        src/drummer/AdaptiveDrummer.cpp
        src/drummer/DrumPattern.cpp
        src/drummer/EnergyAnalyzer.cpp
        src/drummer/DrumSampler.cpp
        src/tracks/TrackBase.cpp
        src/tracks/AudioTrack.cpp
        src/tracks/DrummerTrack.cpp
        src/ui/MainWindow.cpp
        src/ui/TrackView.cpp
        src/ui/TransportBar.cpp
)

target_include_directories(WinBand
    PRIVATE
        src
        ${AUBIO_INCLUDE_DIRS}
)

target_compile_definitions(WinBand
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_ASIO=1
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:WinBand,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:WinBand,JUCE_VERSION>"
)

target_link_libraries(WinBand
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        ${AUBIO_LIBRARIES}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
