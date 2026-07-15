#include "tui.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "formats.hpp"

void Tui::run()
{
    std::cout << "\033[?1049h";

    actions = {
        {"Load model", [this] { load_model(); }},
        {"Train", [this] { train(); }},
        {"Score", [this] { score(); }},
        {"Generate", [this] { generate_track(); }},
        {"Save model", [this] { save_model(); }},
        {"Prune model", [this] { prune_model(); }},
        {"Merge model", [this] { merge_model(); }},
        {"Clear model", [this] { clear(); }},
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
                break;
            }
            if (choice >= 1 && choice <= static_cast<int>(actions.size()))
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

    std::cout << "\033[?1049l";
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

    int trained = 0;
    int total = 0;

    auto train_file = [&](const std::filesystem::path& file_path)
    {
        auto maybe_format = formats::format_from_extension(file_path.extension().string());
        if (!maybe_format)
        {
            return;
        }
        total++;
        formats::Format format = maybe_format.value();
        NoteLoader* loader = formats::get_loader(format);
        if (loader == nullptr)
        {
            return;
        }
        std::ifstream stream(file_path, formats::open_flags(format));
        Track track = notes_to_track(loader->load(stream));
        if (!track.empty())
        {
            model.train(track);
            trained++;
        }
    };

    info_line = "Training...";
    update_screen();

    if (std::filesystem::is_directory(path))
    {
        std::filesystem::directory_iterator dir(path);
        std::size_t file_count = std::distance(begin(dir), end(dir));
        std::size_t current = 0;
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            info_line =
                "Training " + std::to_string(++current) + "/" + std::to_string(file_count) + "...";
            update_screen();
            train_file(file.path());
        }
        info_line =
            "Trained on " + std::to_string(trained) + "/" + std::to_string(total) + " files";
    }
    else
    {
        train_file(path);
        if (trained == 0)
        {
            info_line = "Failed to load file: " + path;
            return;
        }
        info_line = "Training complete";
    }
}

void Tui::generate_track()
{
    static const std::vector<std::pair<std::string, formats::Format>> options = {
        {"MIDI (.mid)", formats::Format::Midi},
        {"ABC (.abc)", formats::Format::Abc},
        {"Plain (.notes)", formats::Format::Plain},
    };

    std::vector<std::string> labels;
    labels.reserve(options.size());
    for (const auto& [label, _] : options)
    {
        labels.push_back(label);
    }

    int choice = ask_option("Select output format", labels);
    if (choice < 0)
    {
        return;
    }
    std::string path = ask_path("Enter output path");
    if (path.empty())
    {
        return;
    }

    if (model.empty())
    {
        info_line = "Model not trained";
        return;
    }
    formats::Format format = options[choice].second;
    info_line = "Generating...";
    update_screen();
    std::mt19937 rng(std::random_device{}());
    Track track = model.generate_track(rng);
    Notes notes = track_to_notes(track);
    std::ofstream file(path, formats::open_flags(format));
    formats::get_saver(format)->save(notes, file);
    info_line = "Generated: " + path;
}

void Tui::score()
{
    if (model.empty())
    {
        info_line = "Model not trained";
        return;
    }
    std::string path = ask_path("Enter path to score", true);
    if (path.empty())
    {
        return;
    }

    auto score_file = [&](const std::filesystem::path& file_path)
    {
        auto maybe_format = formats::format_from_extension(file_path.extension().string());
        if (!maybe_format)
        {
            return -1.0;
        }
        formats::Format format = maybe_format.value();
        NoteLoader* loader = formats::get_loader(format);
        if (loader == nullptr)
        {
            return -1.0;
        }
        std::ifstream stream(file_path, formats::open_flags(format));
        Track track = notes_to_track(loader->load(stream));
        if (track.empty())
        {
            return -1.0;
        }
        return model.evaluate(track);
    };

    info_line = "Scoring...";
    update_screen();

    if (std::filesystem::is_directory(path))
    {
        double sum = 0.0;
        int count = 0;
        info_line = "";
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            double score = score_file(file.path());
            if (score >= 0.0)
            {
                info_line += file.path().filename().string() + ": " + std::to_string(score) + "\n";
                update_screen();
                sum += score;
                count++;
            }
            else
            {
                info_line += file.path().filename().string() + ": FAILED\n";
                update_screen();
            }
        }
        if (count > 0)
        {
            info_line += "Average: " + std::to_string(sum / count);
        }
    }
    else
    {
        double score = score_file(path);
        if (score < 0.0)
        {
            info_line = "Failed to load file: " + path;
            return;
        }
        info_line = "Score: " + std::to_string(score);
    }
}

void Tui::prune_model()
{
    if (model.empty())
    {
        info_line = "Model not trained";
        return;
    }
    auto threshold = ask_int("Enter minimum count threshold");
    if (!threshold)
    {
        return;
    }
    const std::size_t before = model.size();
    info_line = "Pruning...";
    update_screen();
    model.prune(*threshold);
    info_line =
        "Pruned: " + std::to_string(before) + " -> " + std::to_string(model.size()) + " nodes";
}

void Tui::merge_model()
{
    if (model.empty())
    {
        info_line = "Model not trained";
        return;
    }
    std::string path = ask_path("Enter model path to merge", true);
    if (path.empty())
    {
        return;
    }
    info_line = "Merging...";
    update_screen();
    MusicModel other;
    std::ifstream file(path);
    other.load(file);
    if (other.empty())
    {
        info_line = "Failed to load model: " + path;
        return;
    }
    model = model + other;
    info_line = "Model merged: " + path;
}

void Tui::clear()
{
    model = MusicModel{};
    info_line = "Model cleared";
}

void Tui::save_model()
{
    if (model.empty())
    {
        info_line = "Model not trained";
        return;
    }
    std::string path = ask_path("Enter save path");
    if (path.empty())
    {
        return;
    }
    info_line = "Saving...";
    update_screen();
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
    info_line = "Loading...";
    update_screen();
    std::ifstream file(path);
    model.load(file);
    if (model.empty())
    {
        info_line = "Failed to load model: " + path;
        return;
    }
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
        if (choice >= 1 && choice <= static_cast<int>(options.size()))
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

std::optional<int> Tui::ask_int(const std::string& prompt)
{
    info_line = prompt;
    update_screen();
    std::string input;
    std::getline(std::cin, input);
    try
    {
        return std::stoi(input);
    }
    catch (const std::invalid_argument&)
    {
        info_line = "Invalid number";
        return std::nullopt;
    }
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
