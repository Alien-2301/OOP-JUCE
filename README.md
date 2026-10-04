
<h1 align="center">🎧 SynthwavePlayer</h1> <p align="center"> A dual-deck DJ application in C++ with real-time audio DSP, built on the JUCE framework. </p> <p align="center"> <img alt="C++" src="https://img.shields.io/badge/C%2B%2B-14%2B-00599C?logo=c%2B%2B&logoColor=white"> <img alt="JUCE" src="https://img.shields.io/badge/JUCE-6%2F7-8A2BE2"> <img alt="Platform" src="https://img.shields.io/badge/platform-macOS%20(Xcode)-lightgrey?logo=apple"> <img alt="License" src="https://img.shields.io/badge/license-educational%20%2F%20portfolio-blue"> </p> 

<!-- Add a screenshot or short GIF here. This is the single most effective thing for a portfolio README. <p align="center"><img src="docs/demo.gif" alt="SynthwavePlayer demo" width="800"></p> -->

# OOP JUCE Audio Player

A desktop DJ application developed as part of the University of London Object-Oriented Programming module. The project explores object-oriented design, component-based GUI development, audio playback, and basic audio processing using C++ and JUCE.
The app features dual audio decks, waveform visualization, playlist management, hot cues, a 3-band EQ, BPM detection, and persistent per-track settings.


SynthWave DJ Player lets you mix two tracks at once, much like a basic DJ rig. Each deck has its own playback controls, waveform view, 3-band EQ, hot cues and BPM readout. A shared library lets you queue tracks onto either deck, and everything you do (cues, EQ, library) is persisted and restored the next time you open the app.

The project was designed to practise real-time audio programming, clean separation between audio engine and UI, and robust persistence in C++.

## Features
# Dual DJ Decks
- Two independent audio decks
- Play and pause controls
- Volume control
- Playback position control
- Playback speed adjustment
- Load tracks independently into each deck
- Shared audio mixer for simultaneous playback

Playlist Management
- Add multiple audio tracks to a library
- Display:
-- Track title
-- Duration
-- Deck 1 / Deck 2 assignment
- Persistent playlist library
- Supports common audio formats including:
.mp3
.wav
.aiff
.flac
.ogg

## 🧩 OOP Concepts
- Classes and objects
- Encapsulation
- Composition
- Inheritance
- Component-based design
- Separation of responsibilities
- Event-driven programming
- Managing interactions between multiple objects
