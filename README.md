# WinBand

An open-source DAW for Windows, inspired by Apple GarageBand. Aimed at musicians
without a professional DAW background who want to quickly capture ideas on Windows.

---

## Features

| Feature | Details |
|---|---|
| Audio recording | Single track, ASIO (low-latency) or WASAPI |
| Playback & monitoring | Real-time input monitoring with zero-latency path |
| Adaptive drummer | Five styles (Rock / Jazz / Funk / Electronic / Brushes), tap-tempo, guide-track energy analysis |
| Transport | Play / Stop / Record + BPM display with tap-tempo button |
| Track view | Per-track strip: name, mute, solo, VU meter, waveform |
| Theme | Dark UI (bg `#0d0d14`, accent `#e94560`) driven by `assets/theme/dark.json` |

---

## Adaptive Drummer (F5)

The built-in drum generator analyses the energy envelope of a guide track (using
the Aubio library) and adapts its pattern density in real time. It supports five
groove vocabularies and three density levels (Sparse / Medium / Full).

Tap tempo: click the **Tap** button repeatedly; BPM is derived from the average of
the last four taps.

For a standalone VST3 version of the drum engine see
[rutgerwolf/Adaptive-drumming-vst](https://github.com/rutgerwolf/Adaptive-drumming-vst).

---

## UI overview

```
┌──────────────────────────────────────────────────────────────────┐
│  Transport bar                                                   │
│  [ ▶ Play ]  [ ■ Stop ]  [ ● Rec ]   BPM: 120.0  [ Tap ]       │
├──────────────────────────────────────────────────────────────────┤
│  Track 1 — Audio          [M] [S]  ████░░░░  ≈≈≈≈≈≈≈≈≈         │
│  Track 2 — Drummer        [M] [S]  ████████                     │
│  ...                                                             │
└──────────────────────────────────────────────────────────────────┘
  trackHeight 72 px · transportHeight 52 px
```

---

## Platform

| | |
|---|---|
| OS | Windows 10 / 11 64-bit |
| Build system | CMake ≥ 3.22 |
| Compiler | MSVC 2022 or Clang/LLVM (C++17) |
| Audio backend | ASIO (low-latency) + WASAPI fallback |
| JUCE | git submodule (master branch) |
| Aubio | git submodule (master branch) |

---

## Building

```bash
git clone --recurse-submodules https://github.com/rutgerwolf/winband.git
cd winband
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### ASIO support (optional, recommended for low latency)

> ASIO is available free of charge from Steinberg but is **not redistributable**,
> so the SDK is not included in this repository.

1. Download the [ASIO SDK](https://www.steinberg.net/developers/) from Steinberg.
2. Extract it to `third_party/ASIOSDK` (so that
   `third_party/ASIOSDK/common/asio.h` exists).
3. CMake detects the folder automatically and enables the ASIO backend.

Without the SDK, WinBand falls back to WASAPI.

### Salamander Drumkit samples

The drummer requires Salamander WAV files (~2 GB, CC BY 3.0, not in repo).

1. Download from <https://archive.org/details/SalamanderDrumkit>.
2. Place WAV files in `assets/samples/salamander/` using this layout:

```
salamander/
├── kick/
├── snare/
├── hihat/
├── crash/
├── ride/
└── tom/
```

The drummer works without samples but those voices will be silent.

---

## Project structure

```
winband/
├── CMakeLists.txt
├── LICENSES.md
├── assets/
│   ├── theme/dark.json               # Colour palette + UI metrics
│   └── samples/salamander/           # Salamander Drumkit (not in repo)
├── src/
│   ├── main.cpp                      # JUCE_CREATE_APPLICATION entry point
│   ├── MainComponent.h/.cpp          # Root component; wires engine + tracks + UI
│   ├── audio/
│   │   ├── AudioEngine.h/.cpp        # ASIO/WASAPI init, track routing, stereo mix
│   │   └── AsioManager.h/.cpp        # ASIO device management (Windows)
│   ├── drummer/
│   │   ├── AdaptiveDrummer.h/.cpp    # Orchestrator; 5 styles, tap-tempo, energy analysis
│   │   ├── DrumPattern.h/.cpp        # 16-step bitmask grid, 3 densities per style
│   │   ├── DrumSampler.h/.cpp        # Salamander WAV playback
│   │   └── EnergyAnalyzer.h/.cpp     # Aubio RMS pipeline (pure-C++ fallback)
│   ├── tracks/
│   │   ├── TrackBase.h/.cpp          # Abstract base (mute/solo atomics)
│   │   ├── AudioTrack.h/.cpp         # Record + playback + monitoring
│   │   └── DrummerTrack.h/.cpp       # AdaptiveDrummer wrapped as a track
│   └── ui/
│       ├── MainWindow.h/.cpp         # JUCE DocumentWindow
│       ├── TransportBar.h/.cpp       # Play/Stop/Rec/BPM/Tap controls
│       └── TrackView.h/.cpp          # Per-track strip (name, mute, solo, VU, waveform)
├── tests/
│   └── DrumPatternTest.cpp           # JUCE UnitTestRunner (step logic, all styles)
└── third_party/
    ├── JUCE/                         # git submodule
    └── aubio/                        # git submodule
```

---

## Troubleshooting

| Problem | Cause | Fix |
|---|---|---|
| `third_party/JUCE` or `third_party/aubio` empty | Cloned without `--recurse-submodules` | `git submodule update --init --recursive` |
| No ASIO devices in device list | ASIO SDK not found | Place SDK in `third_party/ASIOSDK` (see above) |
| Drummer silent | Salamander WAVs missing | See [Salamander Drumkit samples](#salamander-drumkit-samples) |
| Black / invisible UI | JUCE not initialised before component creation | Ensure `MainComponent` is created after `JUCEApplication::initialise` |
| CMake can't find aubio headers | Submodule not initialised | Same as row 1 |

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
