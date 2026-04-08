cmake_minimum_required(VERSION 3.22)
project(WinBand VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE (git submodule: third_party/JUCE) ───────────────────────────────────────
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/JUCE/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE submodule not found.\n"
        "Run: git submodule update --init --recursive")
endif()
add_subdirectory(third_party/JUCE)

# ── Aubio (git submodule: third_party/aubio) ─────────────────────────────────────
# Aubio is optional: EnergyAnalyzer falls back to pure C++ if not present.
# On Windows, Aubio's own CMakeLists.txt requires pkg-config (a Linux/Unix tool)
# and cannot be used via add_subdirectory.  Skip it on WIN32; the C++ fallback
# in EnergyAnalyzer.cpp (via #if __has_include) handles the rest transparently.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/aubio/CMakeLists.txt" AND NOT WIN32)
    add_subdirectory(third_party/aubio EXCLUDE_FROM_ALL)
    set(AUBIO_LIBRARIES    aubio)
    set(AUBIO_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/aubio/src")
else()
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(AUBIO aubio)  # not REQUIRED — fallback is fine
    endif()
    # If still not found, AUBIO_LIBRARIES and AUBIO_INCLUDE_DIRS remain empty.
    # EnergyAnalyzer uses #if __has_include to detect this at compile time.
endif()

# ── Application sources ─────────────────────────────────────────────────────────
set(WINBAND_SOURCES
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

# Unit tests are compiled into the app (JUCE UnitTestRunner, debug only)
set(WINBAND_TEST_SOURCES
    tests/DrumPatternTest.cpp
)

# ── Application target ──────────────────────────────────────────────────────────
juce_add_gui_app(WinBand
    PRODUCT_NAME     "WinBand"
    COMPANY_NAME     "WinBand Project"
    VERSION          "0.1.0"
    BUNDLE_ID        "nl.winband.app"
    NEEDS_CURL        FALSE
    NEEDS_WEB_BROWSER FALSE
)

target_sources(WinBand
    PRIVATE
        ${WINBAND_SOURCES}
        ${WINBAND_TEST_SOURCES}
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
        # On Windows, avoid pulling in WinRT MIDI which requires the Store SDK
        JUCE_USE_WINRT_MIDI=0
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

# Generate JuceHeader.h (includes all linked JUCE module headers in one file).
# Must be called after target_link_libraries so JUCE knows which modules to include.
juce_generate_juce_header(WinBand)

# ── Post-build: copy assets next to the executable ───────────────────────────────
add_custom_command(TARGET WinBand POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:WinBand>/assets"
    COMMENT "Copying assets to output directory"
)

# ── Developer notes ──────────────────────────────────────────────────────────────
# ASIO SDK:
#   JUCE_ASIO=1 is set above.  JUCE finds the ASIO headers automatically if
#   the Steinberg ASIO SDK is placed alongside the JUCE submodule:
#     third_party/ASIOSDK2.3.3/   (or set JUCE_ASIO_HEADER_PATH in CMake)
#
# Unit tests:
#   Tests are built into WinBand itself and run via juce::UnitTestRunner.
#   To run them, launch WinBand in Debug and call:
#     juce::UnitTestRunner runner;
#     runner.runAllTests();
#   (wire this to a debug menu item in a future iteration)
#
# Salamander Drumkit:
#   Download from https://archive.org/details/SalamanderDrumkit
#   Extract to:  <build_output>/assets/samples/salamander/
#
# GeneralUser GS:
#   Download from http://www.schristiancollins.com/generaluser.php
#   Copy to:     <build_output>/assets/soundfonts/GeneralUserGS.sf2
