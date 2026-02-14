# Markov Music

## Build

Create a build directory inside the project folder and move into it:

```bash
mkdir build
cd build
```

Compile the code:

```bash
cmake ../src && make
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
