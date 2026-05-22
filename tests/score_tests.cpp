#include <array>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <numeric>

#include "midi_processor.hpp"
#include "music_model.hpp"

constexpr std::array<std::string_view, 10> COMPOSERS = {
    "bach",       "beatles",   "beethoven",     "billy_joel", "chopin",
    "elton_john", "hans_zimmer", "john_williams", "mozart",   "taylor_swift"};

double average_score(const MusicModel& model, const std::string& test_folder)
{
    std::vector<double> scores;
    std::cerr.setstate(std::ios::failbit);
    for (const auto& file : std::filesystem::directory_iterator(test_folder))
    {
        Track track = midi_processor::load_track(file.path().string());
        if (!track.empty())
        {
            scores.push_back(model.evaluate(track));
        }
    }
    std::cerr.clear();
    if (scores.empty())
    {
        return 0.0;
    }
    return std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
}

int main()
{
    for (const auto& train_composer : COMPOSERS)
    {
        MusicModel model;
        std::string train_folder = "data/train/" + std::string(train_composer);

        std::cerr.setstate(std::ios::failbit);
        for (const auto& file : std::filesystem::directory_iterator(train_folder))
        {
            Track track = midi_processor::load_track(file.path().string());
            if (!track.empty())
            {
                model.train(track);
            }
        }
        std::cerr.clear();

        std::cout << "Trained on: " << train_composer << "\n";
        double own_score = 0.0;
        for (const auto& test_composer : COMPOSERS)
        {
            std::string test_folder = "data/test/" + std::string(test_composer);
            double score = average_score(model, test_folder);
            std::cout << "  " << test_composer << ": " << score << "\n";
            if (test_composer == train_composer)
            {
                own_score = score;
            }
        }

        double best_other = 0.0;
        for (const auto& test_composer : COMPOSERS)
        {
            if (test_composer == train_composer)
            {
                continue;
            }
            double score = average_score(model, "data/test/" + std::string(test_composer));
            best_other = std::max(best_other, score);
        }

        assert(own_score > best_other);
        std::cout << "  -> PASSED\n\n";
    }

    std::cout << "All tests passed.\n";
    return 0;
}
