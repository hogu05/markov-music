#include "music_model.hpp"

#include <algorithm>
#include <array>

void MusicModel::train(const Track& track)
{
    if (track.empty())
    {
        return;
    }

    std::array<Token, MAX_ORDER> history{};
    history.fill(EMPTY_TOKEN);

    for (const auto& token : track)
    {
        markov_trie.insert(history, token);
        std::shift_left(history.begin(), history.end(), 1);
        history.back() = token;
    }
}

Track MusicModel::generate_track(std::mt19937& rng) const
{
    Track track;

    track.assign(MAX_ORDER, EMPTY_TOKEN);

    Pitch current_pitch = START_PITCH;

    for (;;)
    {
        std::span<const Token> history(track.end() - MAX_ORDER, MAX_ORDER);

        Token next_token = markov_trie.predict(history, current_pitch, rng);

        if (next_token == END_TOKEN)
        {
            break;
        }

        track.push_back(next_token);
        current_pitch += next_token.pitch_delta;
    }

    return {track.begin() + MAX_ORDER, track.end()};
}

constexpr double EPSILON = 1e-10;
double MusicModel::evaluate(const Track& track) const
{
    if (track.empty())
    {
        return 0.0;
    }

    std::array<Token, MAX_ORDER> history{};
    history.fill(EMPTY_TOKEN);

    double score = 0.0;

    Pitch current_pitch = START_PITCH;

    for (const auto& token : track)
    {
        double probability = markov_trie.get_probability(history, current_pitch, token);

        score += std::log(probability + EPSILON);

        std::shift_left(history.begin(), history.end(), 1);
        history.back() = token;
        current_pitch += token.pitch_delta;
    }

    return std::exp(score / static_cast<double>(track.size()));
}
