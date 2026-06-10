# WinBand

An open-source DAW for Windows, inspired by Apple GarageBand. Aimed at musicians without a professional DAW background who want to quickly record ideas on Windows.

## Features (MVP)

- Single audio track recording with ASIO / WASAPI support
- Adaptive drum generator (F5) — five styles, tap tempo, guide-track energy analysis

## Platform

| | |
|---|---|
| OS | Windows 10 / 11 64-bit |
| Build system | CMake >= 3.22 |
| Compiler | MSVC 2022 or Clang/LLVM (C++17) |

## Building

```bash
git clone --recurse-submodules https://github.com/rutgerwolf/winband.git
cd winband
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

> **Note:** ASIO support requires the [Steinberg ASIO SDK](https://www.steinberg.net/developers/).
> Place the extracted SDK in `third_party/ASIOSDK` and set `JUCE_ASIO_HEADER_PATH` in CMake.

## Project structure

```
winband/
├── CMakeLists.txt
├── README.md
├── LICENSES.md
├── assets/
│   ├── theme/dark.json          # JUCE LookAndFeel colour definitions
│   ├── samples/salamander/      # Salamander Drumkit (CC BY 3.0)
│   └── soundfonts/              # GeneralUser GS (MIT)
├── src/
│   ├── main.cpp
│   ├── MainComponent.h/.cpp
│   ├── audio/                   # AudioEngine + ASIO manager
│   ├── drummer/                 # Adaptive drum generator (F5)
│   ├── tracks/                  # Track base classes
│   └── ui/                      # JUCE UI components
├── third_party/                 # git submodules (JUCE, aubio)
└── tests/
```

## Third-party credits

See [LICENSES.md](LICENSES.md) for full licence texts.

| Component | Licence |
|---|---|
| [JUCE](https://juce.com) | GPL v3 |
| [Aubio](https://aubio.org) | GPL v3 |
| [FluidSynth](https://www.fluidsynth.org) | LGPL v2.1 |
| [sfizz](https://sfz.tools/sfizz/) | BSD 2-Clause |
| Salamander Drumkit — Alexander Holm | CC BY 3.0 |
| GeneralUser GS — S. Christian Collins | MIT |

## Licence

WinBand is released under the [GNU General Public Licence v3](LICENSE).
