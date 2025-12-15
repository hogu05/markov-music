# Markov Chain Music Analyzer & Generator

## Introduction

The app will generate and analyze music based on Markov Chain created by uploaded MIDI files.

## Functions

### Training

The user will upload MIDI file with tracks and the app will create Markov Chain based on the music in the tracks

### Anlaysis

The user will upload different MIDI file with track and the app will output a similarity score with the previously uploaded tracks.

### Generation

The app will generate a MIDI file with music based on the created Markov Chain.

## Technologies

- Language: C++23
- Build System: CMake
- External Libraries: [midifile](https://github.com/craigsapp/midifile/tree/master) for working with MIDI files
