# Small Clone Circuit Chorus

An independent, circuit-informed chorus VST3 for Linux, macOS, and Windows. It is not affiliated with MixWave or Electro-Harmonix and does not contain code or image assets from either product. The interface is drawn from native JUCE vector shapes to evoke the pedal faceplate: a rate dial, depth switch, status jewel, footswitch, and a separate Rate Sync control.

## What it models

The circuit reference is the Small Clone based build document linked below. Its bill of materials identifies an MN3007 bucket-brigade delay, CD4047 clock, 4558 and LM358 op amps, and the Rate and Depth controls. The DSP implements a 10 ms nominal modulated delay, a sine LFO, two selectable modulation excursions, a bucket-brigade-style bandwidth limit, input low-frequency filtering, and a dry/wet blend.

This is an approximation of the circuit, not a component-by-component SPICE simulation. The delay line is interpolated rather than a literal 1024-stage analog bucket brigade. The clock, op-amp nonlinearities, component tolerances, and exact response of a particular hardware revision are not modeled. With Sync on and a host tempo available, Rate snaps to the closest musical subdivision; without host tempo, Rate runs freely. MixWave's extra global processing is not included.

Reference material:

- [Small Clone based build document with schematic and component values](https://music-circuits.com/buildinstructions/EHX_SMALL_CLONE_BUILD_DOC_V1.1.pdf)
- [EHX Small Clone operating manual](https://www.ehx.com/wp-content/uploads/2023/03/ehx-small-clone.pdf)
- [MixWave Small Clone user guide](https://support.mixwave.com/help/ehx-small-clone-user-guide)

## Install the prebuilt macOS VST3

Download the latest **macOS Universal VST3** ZIP from the [GitHub Releases page](https://github.com/nickdc8/small-clone-circuit-chorus/releases/latest), then unzip it. Drag the `Small Clone Circuit Chorus.vst3` folder into either:

- `~/Library/Audio/Plug-Ins/VST3` for your user account, or
- `/Library/Audio/Plug-Ins/VST3` for all users.

Restart your DAW or rescan its VST3 plugins. The Universal build supports both Apple Silicon and Intel Macs. No JUCE setup or build step is needed to install this release.

## Build on macOS

Requirements: Xcode Command Line Tools, CMake 3.22+, Git, and an internet connection for JUCE's first download. To build a universal Apple Silicon + Intel VST3, run:

```sh
cmake -S . -B build-mac -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-mac --config Release --target SmallCloneCircuitChorus_VST3 -j
```

JUCE writes the VST3 manifest after its build-time signing step. For a local ad-hoc signature after each rebuild, run:

```sh
codesign --force --deep --sign - \
  "build-mac/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3"
```

The bundle is normally at:

```text
build-mac/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3
```

Install for the current macOS user without administrator privileges:

```sh
mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
cp -R "build-mac/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3" \
  "$HOME/Library/Audio/Plug-Ins/VST3/"
```

For a system-wide install in `/Library/Audio/Plug-Ins/VST3/`, use an administrator account:

```sh
sudo mkdir -p /Library/Audio/Plug-Ins/VST3
sudo cp -R "build-mac/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3" \
  /Library/Audio/Plug-Ins/VST3/
```

To build only for the current Mac's CPU, omit the `CMAKE_OSX_ARCHITECTURES` option.

## Build on Linux

Requirements: CMake 3.22+, a C++17 compiler, Git, and the usual Linux audio/GUI development headers. JUCE is fetched from its official GitHub repository at configure time, so the first build needs internet access.

On Debian/Ubuntu, install the common dependencies:

```sh
sudo apt install build-essential cmake git pkg-config libasound2-dev libjack-jackd2-dev \
  libfreetype6-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev \
  libxext-dev libxinerama-dev libxrandr-dev
```

Then build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target SmallCloneCircuitChorus_VST3 -j
```

The resulting bundle is normally at:

```text
build/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3
```

Install for the current user:

```sh
mkdir -p ~/.vst3
cp -R "build/SmallCloneCircuitChorus_artefacts/Release/VST3/Small Clone Circuit Chorus.vst3" ~/.vst3/
```

To build on Windows, use CMake with Visual Studio and build the `SmallCloneCircuitChorus_VST3` target.

## JUCE licensing

JUCE is fetched as a build dependency and is not vendored here. JUCE is dual-licensed; review [JUCE's current licensing information](https://juce.com/get-juce/) and choose terms that fit your use before distributing a binary. The original DSP and plugin source in this project are provided under the MIT License; see `LICENSE`.
