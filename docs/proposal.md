# Markov Chain Music Analyzer & Generator

## Introduction

The app generates and analyzes music based on Markov Chains.

## Functions

### Training

- The user uploads a MIDI file
- The app processes the tracks from the MIDI file
- The app constructs a Markov Chain based on the tracks

### Analysis

- The user uploads MIDI file
- The app compares the tracks from the uploaded file against the trained Markov Chain
- The app outputs a similarity score for each track

### Generation

- The app generates a track using the trained Markov Chain
- The app outputs a MIDI file with the generated music

## UI

- Terminal User Interface (TUI)

## Technologies

- Language: C++23
- Build System: CMake
- External Libraries: [midifile](https://github.com/craigsapp/midifile/tree/master) for working with MIDI files
