#include "cli.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "formats.hpp"
#include "music_model.hpp"

namespace cli
{

void help()
{
    constexpr int COLUMN_WIDTH = 60;
    std::cout << std::left << std::setw(COLUMN_WIDTH) << "train <input_path> <model_path>"
              << "Train on a file or directory" << std::endl
              << std::setw(COLUMN_WIDTH) << "generate <model_path> <output_path> <format>"
              << "Generate a track (midi, abc, plain)" << std::endl
              << std::setw(COLUMN_WIDTH) << "score <model_path> <input_path>"
              << "Score a file or directory" << std::endl
              << std::setw(COLUMN_WIDTH) << "prune <model_path> <threshold> <output_path>"
              << "Prune rare transitions" << std::endl
              << std::setw(COLUMN_WIDTH) << "merge <model_a> <model_b> <output_path>"
              << "Merge two models" << std::endl
              << std::setw(COLUMN_WIDTH) << "help" << "Show this message" << std::endl
              << std::setw(COLUMN_WIDTH) << "(no arguments)" << "Launch the TUI" << std::endl;
}

MusicModel load_model(const std::string& path)
{
    MusicModel model;
    if (std::filesystem::exists(path))
    {
        std::ifstream file(path);
        model.load(file);
    }
    return model;
}

Track load_track(const std::filesystem::path& file_path)
{
    auto maybe_format = formats::format_from_extension(file_path.extension().string());
    if (!maybe_format)
    {
        return {};
    }
    formats::Format format = maybe_format.value();
    NoteLoader* loader = formats::get_loader(format);
    std::ifstream stream(file_path, formats::open_flags(format));
    return notes_to_track(loader->load(stream));
}

int train(const std::vector<std::string>& args)
{
    if (args.size() != 4)
    {
        std::cerr << "Usage: markov-music train <input_path> <model_path>" << std::endl;
        return 1;
    }
    MusicModel model = load_model(args[3]);
    const std::string& input_path = args[2];
    if (std::filesystem::is_directory(input_path))
    {
        for (const auto& file : std::filesystem::directory_iterator(input_path))
        {
            Track track = load_track(file.path());
            if (!track.empty())
            {
                model.train(track);
            }
        }
    }
    else if (std::filesystem::exists(input_path))
    {
        Track track = load_track(input_path);
        if (track.empty())
        {
            std::cerr << "Failed to load file: " << input_path << std::endl;
            return 1;
        }
        model.train(track);
    }
    if (model.empty())
    {
        std::cerr << "Model not trained" << std::endl;
        return 1;
    }
    std::ofstream file(args[3]);
    model.save(file);
    std::cout << "Model saved: " << args[3] << std::endl;
    return 0;
}

int generate(const std::vector<std::string>& args)
{
    if (args.size() != 5)
    {
        std::cerr << "Usage: markov-music generate <model_path> <output_path> <format>"
                  << std::endl;
        return 1;
    }
    auto maybe_format = formats::format_from_string(args[4]);
    if (!maybe_format)
    {
        std::cerr << "Unknown format: " << args[4] << std::endl;
        return 1;
    }
    formats::Format format = maybe_format.value();
    MusicModel model = load_model(args[2]);
    if (model.empty())
    {
        std::cerr << "Failed to load model: " << args[2] << std::endl;
        return 1;
    }
    std::mt19937 rng(std::random_device{}());
    Track track = model.generate_track(rng);
    std::ofstream file(args[3], formats::open_flags(format));
    formats::get_saver(format)->save(track_to_notes(track), file);
    std::cout << "Generated: " << args[3] << std::endl;
    return 0;
}

int score(const std::vector<std::string>& args)
{
    if (args.size() != 4)
    {
        std::cerr << "Usage: markov-music score <model_path> <input_path>" << std::endl;
        return 1;
    }
    MusicModel model = load_model(args[2]);
    if (model.empty())
    {
        std::cerr << "Failed to load model: " << args[2] << std::endl;
        return 1;
    }
    const std::string& input_path = args[3];
    if (std::filesystem::is_directory(input_path))
    {
        for (const auto& file : std::filesystem::directory_iterator(input_path))
        {
            Track track = load_track(file.path());
            if (!track.empty())
            {
                std::cout << file.path().filename().string() << ": " << model.evaluate(track)
                          << std::endl;
            }
        }
    }
    else
    {
        Track track = load_track(input_path);
        if (track.empty())
        {
            std::cerr << "Failed to load file: " << input_path << std::endl;
            return 1;
        }
        std::cout << model.evaluate(track) << std::endl;
    }
    return 0;
}

int prune(const std::vector<std::string>& args)
{
    if (args.size() != 5)
    {
        std::cerr << "Usage: markov-music prune <model_path> <threshold> <output_path>"
                  << std::endl;
        return 1;
    }
    MusicModel model = load_model(args[2]);
    if (model.empty())
    {
        std::cerr << "Failed to load model: " << args[2] << std::endl;
        return 1;
    }
    int threshold = 0;
    try
    {
        threshold = std::stoi(args[3]);
    }
    catch (const std::invalid_argument&)
    {
        std::cerr << "Invalid threshold: " << args[3] << std::endl;
        return 1;
    }
    model.prune(threshold);
    std::ofstream file(args[4]);
    model.save(file);
    std::cout << "Model saved: " << args[4] << std::endl;
    return 0;
}

int merge(const std::vector<std::string>& args)
{
    if (args.size() != 5)
    {
        std::cerr << "Usage: markov-music merge <model_a> <model_b> <output_path>" << std::endl;
        return 1;
    }
    MusicModel model_a = load_model(args[2]);
    if (model_a.empty())
    {
        std::cerr << "Failed to load model: " << args[2] << std::endl;
        return 1;
    }
    MusicModel model_b = load_model(args[3]);
    if (model_b.empty())
    {
        std::cerr << "Failed to load model: " << args[3] << std::endl;
        return 1;
    }
    MusicModel merged = model_a + model_b;
    std::ofstream file(args[4]);
    merged.save(file);
    std::cout << "Model saved: " << args[4] << std::endl;
    return 0;
}

int run(const std::vector<std::string>& args)
{
    const std::string& command = args[1];

    if (command == "train")
    {
        return train(args);
    }
    if (command == "generate")
    {
        return generate(args);
    }
    if (command == "score")
    {
        return score(args);
    }
    if (command == "prune")
    {
        return prune(args);
    }
    if (command == "merge")
    {
        return merge(args);
    }
    if (command == "help")
    {
        help();
        return 0;
    }

    std::cerr << "Unknown command: " << command << std::endl;
    help();
    return 1;
}

} // namespace cli
