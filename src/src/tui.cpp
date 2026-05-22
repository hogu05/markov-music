#include "tui.hpp"

#include <filesystem>
#include <iostream>

#include "midi_processor.hpp"

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
                reset();
                break;
            case 5:
                show_trained_files();
                break;
            default:
                info_line = "Invalid number, the input has to be a number between 0 and 5";
                break;
            }
        }
        catch (const std::invalid_argument&)
        {
            info_line = "Invalid input, the input has to be a number";
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
    std::cout << "4. Reset model" << std::endl;
    std::cout << "5. Show trained files" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

void Tui::train()
{
    std::string path = ask_path("Enter training path");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }
    if (std::filesystem::is_directory(path))
    {
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            Track track = midi_processor::load_track(file.path().string());
            if (!track.empty())
            {
                model.train(track);
                trained_files.push_back(file.path().filename().string());
            }
        }
    }
    else if (std::filesystem::is_regular_file(path))
    {
        Track track = midi_processor::load_track(path);
        if (!track.empty())
        {
            model.train(track);
            trained_files.push_back(std::filesystem::path(path).filename().string());
        }
    }
    else
    {
        info_line = "Invalid path: " + path;
        return;
    }

    info_line = "Trained on " + std::to_string(trained_files.size()) + " files";
}

void Tui::generate_track()
{
    if (trained_files.empty())
    {
        info_line = "Model not trained";
        return;
    }

    std::string path = ask_path("Enter output path");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }

    std::mt19937 rng(std::random_device{}());
    Track track = model.generate_track(rng);
    midi_processor::save_track(track, path);
    info_line = "Generated: " + path;
}

void Tui::score()
{
    if (trained_files.empty())
    {
        info_line = "Model not trained";
        return;
    }

    std::string path = ask_path("Enter path to score");
    if (path.empty())
    {
        info_line = "Cancelled";
        return;
    }

    info_line = "";

    if (std::filesystem::is_directory(path))
    {
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            // TODO: look at this more
            std::cerr.setstate(std::ios::failbit);
            Track track = midi_processor::load_track(file.path().string());
            std::cerr.clear();

            if (!track.empty())
            {
                double score = model.evaluate(track);
                info_line += file.path().filename().string() + ": " + std::to_string(score) + "\n";
            }
        }
    }
    else
    {
        Track track = midi_processor::load_track(path);
        if (track.empty())
        {
            info_line = "Failed to load file: " + path;
            return;
        }
        info_line = "Score: " + std::to_string(model.evaluate(track));
    }
}

void Tui::reset()
{
    model = MusicModel{};
    trained_files.clear();
    info_line = "Model reset";
}

std::string Tui::ask_path(const std::string& prompt)
{
    info_line = prompt;
    update_screen();
    std::string path;
    std::getline(std::cin, path);
    return path;
}

void Tui::show_trained_files()
{
    if (trained_files.empty())
    {
        info_line = "No files trained";
        return;
    }

    info_line = "";
    for (const auto& file : trained_files)
    {
        info_line += file + "\n";
    }
}
