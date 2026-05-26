#include <array>
#include <cassert>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>

#include "midi_processor.hpp"
#include "music_model.hpp"

constexpr std::array<std::string, 10> ARTISTS = {
    "bach",        "beethoven",     "billy_joel",      "chopin", "elton_john",
    "hans_zimmer", "john_williams", "michael_jackson", "mozart", "taylor_swift"};

const std::vector<size_t> ALL = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const std::vector<size_t> CLASSICAL = {0, 1, 3, 8};
const std::vector<size_t> POP = {7, 2, 4, 9};

std::vector<double> score(const std::vector<size_t>& train_ids, const std::vector<size_t>& test_ids)
{
    MusicModel model;
    for (size_t id : train_ids)
    {
        for (const auto& file : std::filesystem::directory_iterator("data/train/" + ARTISTS.at(id)))
        {
            Track track = midi_processor::load_track(file.path().string());
            if (!track.empty())
            {
                model.train(track);
            }
        }
    }

    std::vector<double> scores(ARTISTS.size());
    for (size_t id : test_ids)
    {
        double sum = 0.0;
        int count = 0;
        for (const auto& file : std::filesystem::directory_iterator("data/test/" + ARTISTS.at(id)))
        {
            Track track = midi_processor::load_track(file.path().string());
            if (!track.empty())
            {
                sum += model.evaluate(track);
                count++;
            }
        }
        scores[id] = count > 0 ? sum / count : 0.0;
    }
    return scores;
}

double average(const std::vector<double>& scores, const std::vector<size_t>& ids)
{
    double sum = 0.0;
    for (size_t id : ids)
    {
        sum += scores[id];
    }
    return sum / static_cast<double>(ids.size());
}

int main()
{
    std::cout << "ARTIST RECOGNITION" << std::endl;
    std::cout << std::left << std::setw(18) << "Artist" << "| " << std::setw(12) << "Score"
              << "| " << std::setw(12) << "Other" << "| " << "Result" << std::endl;
    std::cout << std::string(58, '-') << std::endl;

    bool artist_recognition_passed = true;
    for (size_t artist_id = 0; artist_id < ARTISTS.size(); ++artist_id)
    {
        auto scores = score({artist_id}, ALL);
        double artist_score = scores[artist_id];
        double best_other = 0.0;
        for (size_t other_id = 0; other_id < ARTISTS.size(); ++other_id)
        {
            if (other_id != artist_id)
            {
                best_other = std::max(best_other, scores[other_id]);
            }
        }

        bool passed = artist_score > best_other;
        artist_recognition_passed = artist_recognition_passed && passed;

        std::cout << std::left << std::setw(18) << ARTISTS.at(artist_id) << "| " << std::setw(12)
                  << std::setprecision(5) << artist_score << "| " << std::setw(12) << best_other
                  << "| " << (passed ? "PASS" : "FAIL") << std::endl;
    }

    std::cout << (artist_recognition_passed ? "PASSED" : "FAILED") << std::endl;

    std::cout << std::endl;

    std::cout << "GENRE RECOGNITION" << std::endl;
    auto scores = score(CLASSICAL, ALL);

    double classical_avg = average(scores, CLASSICAL);
    double pop_avg = average(scores, POP);

    std::cout << std::left << std::setw(18) << "Classical avg" << classical_avg << std::endl;
    std::cout << std::left << std::setw(18) << "Pop avg" << pop_avg << std::endl;
    std::cout << (classical_avg > pop_avg ? "PASSED" : "FAILED") << std::endl;
    return 0;
}
