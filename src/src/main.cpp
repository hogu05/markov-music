#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "midi_processor.hpp"
#include "music_model.hpp"
#include "types.hpp"

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error: Not enough arguments" << std::endl;
        return 1;
    }

    std::string training_folder = argv[1];

    if (!std::filesystem::exists(training_folder))
    {
        std::cerr << "Error: Training folder not found at: "
                  << std::filesystem::absolute(training_folder) << std::endl;
        return 1;
    }

    MusicModel model;
    for (const auto& file : std::filesystem::directory_iterator(training_folder))
    {
        std::vector<Token> tokens = midi_processor::load_tokens(file.path().string());

        if (!tokens.empty())
        {
            model.train(tokens);
        }
    }

    std::string mode = argv[2];
    if (mode == "generate")
    {
        std::mt19937 rng(std::random_device{}());
        std::vector<Token> tokens = model.generate(rng);
        midi_processor::save_tokens(tokens, argv[3]);
    }
    else if (mode == "score")
    {
        std::vector<Token> tokens = midi_processor::load_tokens(argv[3]);
        double score = model.evaluate(tokens);
        std::cout << "Score: " << score << std::endl;
    }
    else
    {
        std::cerr << "Error: Unknown mode: " << mode << std::endl;
    }

    return 0;
}

// TODO ability to change output syle: major/minor, F, G ...
// TODO maybe temperature, clean ending - have END note with increasing probability
