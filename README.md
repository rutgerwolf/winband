# WinBand

An open-source DAW for Windows, inspired by Apple GarageBand. Aimed at musicians
without a professional DAW background who want to quickly capture ideas on Windows.

---

## Features

| Feature | Details |
|---|---|
| Audio recording | Single track, ASIO (low-latency) or WASAPI |
| Playback & monitoring | Real-time input monitoring |
| Adaptive drummer | Five styles (Rock / Jazz / Funk / Electronic / Brushes), tap-tempo, guide-track energy analysis |
| Transport | Play / Stop / Record + BPM display with tap-tempo button |
| Track view | Per-track strip: name, mute, solo, VU meter, waveform |
| Theme | Dark UI driven by `assets/theme/dark.json` |

---

## Adaptive Drummer

The built-in drum generator analyses the energy envelope of a guide track (via
Aubio) and adapts its pattern density in real time. Five groove vocabularies,
three density levels (Sparse / Medium / Full), tap-tempo.

For a standalone VST3 version see
[rutgerwolf/Adaptive-drumming-vst](https://github.com/rutgerwolf/Adaptive-drumming-vst).

---

## UI overview

```
┌──────────────────────────────────────────────────────────────────┐
│  Transport  [ ▶ Play ]  [ ■ Stop ]  [ ● Rec ]  120.0 BPM  [ Tap ]  │
├──────────────────────────────────────────────────────────────────┤
│  Track 1 — Audio Track     [M] [S]  ████░░░░  ≈≈≈≈≈≈≈≈≈             │
│  Track 2 — Drum Track      [M] [S]  ████████                         │
└──────────────────────────────────────────────────────────────────┘
  trackHeight 72 px · transportHeight 52 px
```

---

## Platform

| | |
|---|---|
| OS | Windows 10 / 11 64-bit |
| Build system | CMake ≥ 3.22 |
| Compiler | MSVC 2022 (C++17) |
| Audio backend | ASIO (low-latency) + WASAPI fallback |
| JUCE | git submodule (master branch) |
| Aubio | git submodule (master branch) |

---

## Building on Windows

### Prerequisites

| Tool | Download | Notes |
|---|---|---|
| Git for Windows | <https://git-scm.com/download/win> | Required for submodule support |
| CMake 3.22+ | <https://cmake.org/download/> | Tick “Add to PATH” during install |
| Visual Studio 2022 | <https://visualstudio.microsoft.com/> | Workload: **Desktop development with C++** |

### Step 1 — Clone (including submodules)

```bat
git clone --recurse-submodules https://github.com/rutgerwolf/winband.git
cd winband
```

If you already cloned without `--recurse-submodules`:

```bat
git submodule update --init --recursive
```

### Step 2 — (Optional) Install the ASIO SDK

ASIO gives significantly lower audio latency than WASAPI. Without it WinBand still
builds and runs via WASAPI.

1. Download the free [ASIO SDK](https://www.steinberg.net/developers/) from Steinberg.
2. Extract it so that `third_party\ASIOSDK\common\asio.h` exists.
3. CMake detects the folder automatically and enables the ASIO backend.

### Step 3 — Configure

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
```

### Step 4 — Build

```bat
cmake --build build --config Release
```

### Step 5 — Output

| Artifact | Path |
|---|---|
| WinBand executable | `build\Release\WinBand.exe` |

Run it directly from that folder so that relative paths to `assets\` resolve correctly.

---

## Salamander Drumkit samples

The drummer requires Salamander WAV files (~2 GB, CC BY 3.0, not included in repo).

1. Download from <https://archive.org/details/SalamanderDrumkit>.
2. Extract and place the WAV files in `assets\samples\salamander\` using this layout:

```
assets\samples\salamander\
├── kick\          e.g. kick_OH_F_1.wav, kick_OH_MP_1.wav …
├── snare\
├── hihat\
├── crash\
├── ride\
└── tom\
```

WinBand loads samples on startup from that path. Missing folders are silently
skipped (that drum voice will be silent).

---

## Project structure

```
winband\
├── CMakeLists.txt
├── LICENSES.md
├── assets\
│   ├── theme\dark.json               # Colour palette + UI metrics
│   └── samples\salamander\           # Salamander Drumkit (not in repo)
├── src\
│   ├── main.cpp
│   ├── MainComponent.h/.cpp
│   ├── audio\
│   │   ├── AudioEngine.h/.cpp        # ASIO/WASAPI init, track routing, stereo mix
│   │   └── AsioManager.h/.cpp        # ASIO device management
│   ├── drummer\
│   │   ├── AdaptiveDrummer.h/.cpp    # Orchestrator; 5 styles, tap-tempo, energy analysis
│   │   ├── DrumPattern.h/.cpp        # 16-step bitmask grid, 3 densities per style
│   │   ├── DrumSampler.h/.cpp        # Salamander WAV playback
│   │   └── EnergyAnalyzer.h/.cpp     # Aubio RMS pipeline (pure-C++ fallback)
│   ├── tracks\
│   │   ├── TrackBase.h/.cpp
│   │   ├── AudioTrack.h/.cpp
│   │   └── DrummerTrack.h/.cpp
│   └── ui\
│       ├── MainWindow.h/.cpp
│       ├── TransportBar.h/.cpp
│       └── TrackView.h/.cpp
├── tests\
│   └── DrumPatternTest.cpp
└── third_party\
    ├── JUCE\                         # git submodule
    └── aubio\                        # git submodule
```

---

## Troubleshooting

| Problem | Cause | Fix |
|---|---|---|
| `third_party\JUCE` or `third_party\aubio` empty | Cloned without `--recurse-submodules` | `git submodule update --init --recursive` |
| No ASIO devices listed | ASIO SDK not found | Place SDK at `third_party\ASIOSDK` (see above) |
| Drummer silent | Salamander WAVs missing | See [Salamander Drumkit samples](#salamander-drumkit-samples) |
| Black / invisible UI on startup | JUCE display init race | Ensure no DPI scaling issues; try running as administrator once |
| CMake error about missing headers | Stale cache after submodule init | Delete `build\` and rerun configure |

---

## Third-party credits

See [LICENSES.md](LICENSES.md) for full licence texts.

| Component | Licence |
|---|---|
| [JUCE](https://juce.com) | GPL v3 / JUCE commercial licence |
| [Aubio](https://aubio.org) | GPL v3 |
| Salamander Drumkit — Alexander Holm | CC BY 3.0 |

## Licence

WinBand is released under the [GNU General Public Licence v3](LICENSE).
