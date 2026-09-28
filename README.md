# Markov Music - User Documentation

Tool for training a Markov chain model on tracks, generating new music, and evaluating how well tracks fit the model.

## Requirements

### Hardware

- **RAM**: 2 GB minimum
- **Storage**: ~150 MB per model

### Software

- **CMake**: 3.30+
- **Compiler**: C++23
- **OS**: Windows, macOS, Linux
- **MIDI player** _(optional)_: for playing MIDI files (e.g. timidity)

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

> The model generates piano tracks. For best results, train on piano music.

### TUI

Run without arguments to launch the TUI:

```bash
./build/markov-music
```

The program presents a menu:

- **1. Load model** - load a previously saved model
- **2. Train** - train the model on a file or directory
- **3. Score** - score a file or directory based on how well it fits the model
- **4. Generate** - generate a track and save it to a file
- **5. Save model** - save the current model to a file
- **6. Prune model** - remove rare transitions to reduce model size
- **7. Merge model** - merge another model into the current one
- **8. Clear model** - clear the current model
- **0. Quit** - quit the program

### CLI

Pass a command as an argument:

```bash
./build/markov-music <command> [arguments]
```

| Command    | Arguments                                | Description                         |
| ---------- | ---------------------------------------- | ----------------------------------- |
| `train`    | `<input_path> <model_path>`              | Train on a file or directory        |
| `generate` | `<model_path> <output_path> <format>`    | Generate a track (midi, abc, plain) |
| `score`    | `<model_path> <input_path>`              | Score a file or directory           |
| `prune`    | `<model_path> <threshold> <output_path>` | Prune rare transitions              |
| `merge`    | `<model_a> <model_b> <output_path>`      | Merge two models                    |
| `help`     |                                          | Show available commands             |

#### Examples

```bash
./build/markov-music train data/train/bach bach.model

./build/markov-music generate bach.model output.mid midi

./build/markov-music score bach.model data/test/bach

./build/markov-music prune bach.model 5 bach_pruned.model

./build/markov-music merge bach.model mozart.model merged.model
```

## Formats

| Format | Extension | Description          |
| ------ | --------- | -------------------- |
| MIDI   | `.mid`    | Standard MIDI format |
| ABC    | `.abc`    | ABC music notation   |
| Plain  | `.notes`  | Custom text format   |

## Data

The `data/` directory contains MIDI piano tracks of 10 artists, split into `train/` (hundreds per artist) and `test/` (20 per artist) with no overlap.
The included artists are:

- Bach, Beethoven, Chopin, Mozart
- Hans Zimmer, John Williams
- Billy Joel, Elton John, Michael Jackson, Taylor Swift

Tracks are sourced from [The aria-MIDI Dataset](https://github.com/loubbrad/aria-midi).

## Tests

### Unit tests

Run the unit tests from the project root:

```bash
./build/unit-tests
```

Tests format round trips and model save/load.

### Score tests

Run the score tests from the project root:

```bash
./build/score-tests
```

Tests artist and genre recognition.
Should take a few minutes.
