#include "tui.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "midi_loader.hpp"
#include "midi_saver.hpp"

void Tui::run()
{
    info_line = "Welcome to Markov Music!";

    for (;;)
    {
        update_screen();

        std::string input;
        std::getline(std::cin, input);
        try
        {
            switch (std::stoi(input))
            {
            case 0:
                return;
            case 1:
                train();
                break;
            case 2:
                generate_track();
                break;
            case 3:
                score();
                break;
            case 4:
                clear();
                break;
            default:
                info_line = "Invalid option";
                break;
            }
        }
        catch (const std::invalid_argument&)
        {
            info_line = "Enter a number";
        }
    }
}

void Tui::clear_screen()
{
    std::cout << "\033[2J\033[H";
}

void Tui::update_screen()
{
    clear_screen();
    print_menu();
    std::cout << std::endl;
    std::cout << info_line << std::endl;
    std::cout << "> ";
}

void Tui::print_menu()
{
    std::cout << "1. Train" << std::endl;
    std::cout << "2. Generate" << std::endl;
    std::cout << "3. Score" << std::endl;
    std::cout << "4. Clear model" << std::endl;
    std::cout << "0. Quit" << std::endl;
}

void Tui::train()
{
    std::string path = ask_path("Enter training path");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }
    if (!std::filesystem::exists(path))
    {
        info_line = "Invalid path: " + path;
        return;
    }

    info_line = "Training...";
    update_screen();
    if (std::filesystem::is_directory(path))
    {
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            std::ifstream midi_stream(file.path(), std::ios::binary);
            Track track = notes_to_track(MidiLoader{}.load(midi_stream));
            if (!track.empty())
            {
                model.train(track);
            }
        }
    }
    else
    {
        std::ifstream midi_stream(path, std::ios::binary);
        Track track = notes_to_track(MidiLoader{}.load(midi_stream));
        if (track.empty())
        {
            info_line = "Failed to load file: " + path;
            return;
        }
        model.train(track);
    }
    info_line = "Training complete";
}

void Tui::generate_track()
{
    std::string path = ask_path("Enter output path");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }

    info_line = "Generating...";
    update_screen();
    std::mt19937 rng(std::random_device{}());
    Track track = model.generate_track(rng);
    if (track.empty())
    {
        info_line = "Model not trained";
        return;
    }
    std::ofstream midi_stream(path, std::ios::binary);
    MidiSaver{}.save(track_to_notes(track), midi_stream);
    info_line = "Generated: " + path;
}

void Tui::score()
{
    std::string path = ask_path("Enter path to score");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }
    if (!std::filesystem::exists(path))
    {
        info_line = "Invalid path: " + path;
        return;
    }

    info_line = "Scoring...";
    update_screen();

    if (std::filesystem::is_directory(path))
    {
        double sum = 0.0;
        int count = 0;
        info_line = "";
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            std::ifstream midi_stream(file.path(), std::ios::binary);
            Track track = notes_to_track(MidiLoader{}.load(midi_stream));
            if (!track.empty())
            {
                double score = model.evaluate(track);
                info_line += file.path().filename().string() + ": " + std::to_string(score) + "\n";
                update_screen();
                sum += score;
                count++;
            }
        }

        if (count > 0)
        {
            info_line += "Average: " + std::to_string(sum / count);
        }
    }
    else
    {
        std::ifstream midi_stream(path, std::ios::binary);
        Track track = notes_to_track(MidiLoader{}.load(midi_stream));
        if (track.empty())
        {
            info_line = "Failed to load file: " + path;
            return;
        }
        info_line = "Score: " + std::to_string(model.evaluate(track));
    }
}

void Tui::clear()
{
    model = MusicModel{};
    info_line = "Model cleared";
}

std::string Tui::ask_path(const std::string& prompt)
{
    info_line = prompt;
    update_screen();
    std::string path;
    std::getline(std::cin, path);
    return path;
}
