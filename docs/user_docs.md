# Markov Music

## Build

### Linux/macOS

```bash
./install.sh
```

### Windows

```bash
.\install.bat
```

## Usage

Inside the `build` directory, run the program with:

```bash
./markov-music <training_dir> <mode> <target_file>
```

## Modes

### `generate`

Trains a model on the midi files in the training directory. Generates a new song with the trained model.

#### Example

```bash
./markov-music ../data/bach generate out.midi
```

### `score`

Evaluates similarity between target file and the trained model from training directory.

#### Example

```bash
./markov-music ../data/bach score ../data/test/beatles.mid
```

# Not implemented features

- GUI
- Songs with multiple instruments
