#include <iostream>

#include "tui.hpp"

int main(int argc, char* argv[])
{
    /*if (argc < 4)
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
        std::vector<Token> tokens = midi_processor::load_track(file.path().string());

        if (!tokens.empty())
        {
            model.train(tokens);
        }
    }

    std::string mode = argv[2];
    if (mode == "generate")
    {
        std::mt19937 rng(std::random_device{}());
        std::vector<Token> tokens = model.generate_track(rng);
        midi_processor::save_track(tokens, argv[3]);
    }
    else if (mode == "score")
    {
        std::vector<Token> tokens = midi_processor::load_track(argv[3]);
        double score = model.evaluate(tokens);
        std::cout << "Score: " << score << std::endl;
    }
    else
    {
        std::cerr << "Error: Unknown mode: " << mode << std::endl;
    }*/
    std::cout << "\033[?1049h";
    Tui tui;
    tui.run();
    std::cout << "\033[?1049l";

    return 0;
}

// TODO: maybe temperature, clean ending - have END note with increasing probability
// TODO: TUI
// TODO: multiple instruments
// TODO: tests
