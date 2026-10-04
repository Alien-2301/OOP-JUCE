
<h1 align="center">🎧 SynthwavePlayer</h1> <p align="center"> A dual-deck DJ application in C++ with real-time audio DSP, built on the JUCE framework. </p> <p align="center"> <img alt="C++" src="https://img.shields.io/badge/C%2B%2B-14%2B-00599C?logo=c%2B%2B&logoColor=white"> <img alt="JUCE" src="https://img.shields.io/badge/JUCE-6%2F7-8A2BE2"> <img alt="Platform" src="https://img.shields.io/badge/platform-macOS%20(Xcode)-lightgrey?logo=apple"> <img alt="License" src="https://img.shields.io/badge/license-educational%20%2F%20portfolio-blue"> </p> 

<!-- Add a screenshot or short GIF here. This is the single most effective thing for a portfolio README. <p align="center"><img src="docs/demo.gif" alt="SynthwavePlayer demo" width="800"></p> -->

# OOP JUCE Audio Player

SynthwavePlayer lets you mix two tracks at once, much like a basic DJ rig. Each deck has its own playback controls, waveform view, 3-band EQ, hot cues and BPM readout. A shared library lets you send tracks to either deck, and your cues, EQ settings and library are saved and restored the next time you open the app.

It was built from the ground up, without a high-level player library, to explore real-time audio programming, DSP, and clean separation between audio engine, UI and persistence in C++.

## 🧩 OOP Concepts
- Classes and objects
- Encapsulation
- Composition
- Inheritance
- Component-based design
- Separation of responsibilities
- Event-driven programming
- Managing interactions between multiple objects

## Features
- **Real-time audio engine:** a custom `juce::AudioSource` chains file reading, transport, variable-rate sampling and DSP, and mixes two decks into one output.
- **3-band EQ**:
  - low shelf: 250 Hz
  - mid shelf: 1 kHz
  - high shelf: 5 kHz
- **Variable-speed playback:** ranges between 0.5x to 2.0x for Resampling Audio Source
- **BPM detection:** block-based RMS energy analysis with peak picking, scaled live by the playback speed.
- **Hot cues:** set, jump and clear cue points, saved per track.
Per-track memory: EQ and cues are keyed by file path and reapplied when a track is loaded again.
- **Library:** multi-file import (`.mp3`, `.wav`, `.aiff`, `.flac`, `.ogg`) with title and duration, one-click loading into either deck, persisted between sessions.
- **Waveform display:** `AudioThumbnail` waveform with a live playhead.
- **Drag and drop**: drop an audio file onto a deck to load it.

## Application Layout
<img width="1025" height="588" alt="image" src="https://github.com/user-attachments/assets/c4f0a51b-cd72-426c-be8d-9491be6e77de" />

## Dual DJ process breakdown
<img width="570" height="615" alt="image" src="https://github.com/user-attachments/assets/8a97bbfd-f4fe-46d2-b386-288973ee184c" />

| Class / File Pair | Category | Main Responsibilities | Key Interactions |
| :--- | :--- | :--- | :--- |
| **MainComponent.h** /<br>**MainComponent.cpp** | Application Controller | Initializes audio system, connects decks to mixer, manages audio device, coordinates major components | • DeckGUI<br>• DJAudioPlayer<br>• MixerAudioSource<br>• AudioDeviceManager |
| **DeckGUI.h** /<br>**DeckGUI.cpp** | Deck Interface Controller | Handles deck UI, user interaction (buttons, sliders), loads tracks, communicates playback commands to audio player | • DJAudioPlayer<br>• WaveformDisplay<br>• CueManager<br>• EQManager |
| **DJAudioPlayer.h** /<br>**DJAudioPlayer.cpp** | Audio Processing Engine | Loads audio files, controls playback, manages gain/speed/position, BPM analysis, processes audio blocks, applies DSP filters | • MixerAudioSource<br>• DeckGUI<br>• AudioFormatReaderSource |
| **WaveformDisplay.h** /<br>**WaveformDisplay.cpp** | Audio Visualization Component | Renders waveform, displays playback position, visual feedback for track progress | • DeckGUI<br>• DJAudioPlayer<br>• AudioThumbnail |
| **PlayListComponent.h** /<br>**PlayListComponent.cpp** | Playlist Manager UI | Displays music library, allows user to select tracks and load them into decks | • DeckGUI<br>• File system / library data |
| **CueManager.h** /<br>**CueManager.cpp** | Cue Point Management Service | Handles cue creation, storage, retrieval, loads and saves cue profiles to CSV | • DeckGUI<br>• File system (`SynthWavePlayerCue.csv`) |
| **EQManager.h** /<br>**EQManager.cpp** | Equalizer Profile Manager | Stores EQ settings, loads/saves EQ profiles, applies EQ parameters to the audio player | • DeckGUI<br>• DJAudioPlayer<br>• File system (`SynthWavePlayerEQ.csv`) |
| **CueButton.h** /<br>**CueButton.cpp** *(if present)* | UI Control Component | Represents individual cue buttons, interacts with cue system to set/jump to cue points | • DeckGUI<br>• CueManager |
| **LibraryManager.h** /<br>**LibraryManager.cpp** *(if present)* | Track Library Manager | Maintains track metadata, loads library from CSV, supports playlist UI | • PlayListComponent<br>• File system (`dj_library.csv`) |

## Design notes

- **Engine and UI are decoupled**. `DJAudioPlayer` knows nothing about widgets; DeckGUI only uses its public interface.
- **Persistence lives in dedicated managers.** Each owns a `std::map` keyed by track path and syncs it to disk, keeping file `I/O` out of both the UI and the audio code.
- **Component recycling is respected**. The playlist reuses row buttons in refreshComponentForCell, tagging them with a `row:column ID`, so scrolling stays cheap.
- **RAII for resources**. Readers and sources are held in `std::unique_ptr`, and components use JUCE's leak detector.
