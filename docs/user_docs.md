# Markov Music - User Documentation

Tool for training a Markov chain model on tracks, generating new music, and evaluating how well tracks fit the model.

## Requirements

- CMake 3.30+
- C++23 compiler

## Build

Run the install script from the project root:

### Linux/macOS

```bash
./install.sh
```

### Windows

```bat
.\install.bat
```

## Usage

Run the program from the project root:

```bash
./build/markov-music
```

### TUI

The program presents a menu:

- **1. Train** - train the model on a MIDI file/directory

> The model generates piano tracks. For best results, train on piano music.

- **2. Generate** - generate a MIDI file based on the trained model
- **3. Score** - score a MIDI file/directory based on how well the tracks fit the model
- **4. Clear model** - clear the model
- **0. Quit** - quit the program

> When asked for a path, leaving the path empty cancels the current operation.

## Data

The `data/` directory contains MIDI piano tracks of 10 artists, split into `train/` (hundreds per artist) and `test/` (20 per artist) with no overlap.
The included artists are:

- Bach, Beethoven, Chopin, Mozart
- Hans Zimmer, John Williams
- Billy Joel, Elton John, Michael Jackson, Taylor Swift

The tracks can be used in the TUI, for example, train on `data/train/bach` and score `data/test/bach`.

Tracks are sourced from [The aria-MIDI Dataset](https://github.com/loubbrad/aria-midi).

## Tests

Run the tests from the project root:

```bash
./build/score-tests
```

Two tests are run:

- **Artist recognition** - trains on each artist and checks that the artist's own tracks score higher than any other artist's
- **Genre recognition** - trains on classical composers and checks that their tracks score higher on average than pop tracks

The tests should take a few minutes.

TODO: Windows
