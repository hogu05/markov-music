#include "tui.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "abc_saver.hpp"
#include "midi_loader.hpp"
#include "midi_saver.hpp"
#include "note_saver.hpp"

void Tui::run()
{
    actions = {
        {"Load model", [this] { load_model(); }}, {"Train", [this] { train(); }},
        {"Score", [this] { score(); }},           {"Generate", [this] { generate_track(); }},
        {"Save model", [this] { save_model(); }}, {"Clear model", [this] { clear(); }},
    };

    info_line = "Welcome to Markov Music!";

    for (;;)
    {
        update_screen();

        std::string input;
        std::getline(std::cin, input);
        try
        {
            int choice = std::stoi(input);
            if (choice == 0)
            {
                return;
            }
            if (choice >= 1 && choice <= actions.size())
            {
                actions[choice - 1].second();
            }
            else
            {
                info_line = "Invalid option";
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

void Tui::print_menu() const
{
    for (std::size_t i = 0; i < actions.size(); ++i)
    {
        std::cout << (i + 1) << ". " << actions[i].first << std::endl;
    }
    std::cout << "0. Quit" << std::endl;
}

void Tui::train()
{
    std::string path = ask_path("Enter training path", true);
    if (path.empty())
    {
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
    static MidiSaver midi_saver;
    static AbcSaver abc_saver;
    static const std::vector<std::pair<std::string, NoteSaver*>> savers = {
        {"MIDI", &midi_saver},
        {"ABC", &abc_saver},
    };

    std::vector<std::string> labels;
    labels.reserve(savers.size());
    for (const auto& [label, _] : savers)
    {
        labels.push_back(label);
    }

    int format = ask_option("Select output format", labels);
    if (format < 0)
    {
        return;
    }
    std::string path = ask_path("Enter output path");
    if (path.empty())
    {
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
    Notes notes = track_to_notes(track);
    std::ofstream file(path, format == 0 ? std::ios::binary : std::ios::out);
    savers[format].second->save(notes, file);
    info_line = "Generated: " + path;
}

void Tui::score()
{
    // TODO: option
    std::string path = ask_path("Enter path to score", true);
    if (path.empty())
    {
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

void Tui::save_model()
{
    std::string path = ask_path("Enter save path");
    if (path.empty())
    {
        return;
    }
    std::ofstream file(path);
    model.save(file);
    info_line = "Model saved: " + path;
}

void Tui::load_model()
{
    std::string path = ask_path("Enter model path", true);
    if (path.empty())
    {
        return;
    }
    std::ifstream file(path);
    model.load(file);
    info_line = "Model loaded: " + path;
}

int Tui::ask_option(const std::string& prompt, const std::vector<std::string>& options)
{
    info_line = prompt + "\n";
    for (std::size_t i = 0; i < options.size(); ++i)
    {
        info_line += std::to_string(i + 1) + ". " + options[i] + "\n";
    }
    info_line += "0. Cancel";
    update_screen();

    std::string input;
    std::getline(std::cin, input);
    try
    {
        int choice = std::stoi(input);
        if (choice == 0)
        {
            info_line = "Cancelled";
            return -1;
        }
        if (choice >= 1 && choice <= options.size())
        {
            return choice - 1;
        }
    }
    catch (const std::invalid_argument&)
    {
    }
    info_line = "Invalid option";
    return -1;
}

std::string Tui::ask_path(const std::string& prompt, bool check)
{
    info_line = prompt;
    update_screen();
    std::string path;
    std::getline(std::cin, path);
    if (path.empty())
    {
        info_line = "Cancelled";
        return "";
    }
    if (check && !std::filesystem::exists(path))
    {
        info_line = "Invalid path: " + path;
        return "";
    }
    return path;
}
