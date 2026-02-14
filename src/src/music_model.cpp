#include "music_model.hpp"

#include <algorithm>
#include <array>

void MusicModel::train(const std::vector<Token>& tokens)
{
    if (tokens.empty())
    {
        return;
    }

    std::array<Token, MAX_ORDER> history{};
    history.fill(START_TOKEN);

    for (const auto& token : tokens)
    {
        markov_trie.insert(history, token);
        std::shift_left(history.begin(), history.end(), 1);
        history.back() = token;
    }
}

constexpr int MAX_GENERATE_LENGTH = 1000;
std::vector<Token> MusicModel::generate(std::mt19937& rng) const
{
    std::vector<Token> tokens;

    tokens.reserve(MAX_ORDER);
    for (int i = 0; i < MAX_ORDER; ++i)
    {
        tokens.push_back(START_TOKEN);
    }

    Pitch current_pitch = START_PITCH;

    for (int i = 0; i < MAX_GENERATE_LENGTH; ++i)
    {
        std::span<const Token> history(tokens.end() - MAX_ORDER, MAX_ORDER);

        Token next_token = markov_trie.predict(history, current_pitch, rng);

        if (next_token == END_TOKEN)
        {
            break;
        }

        tokens.push_back(next_token);
        current_pitch += next_token.pitch_delta;
    }

    return tokens;
}

constexpr double EPSILON = 1e-5;
double MusicModel::evaluate(const std::vector<Token>& song) const
{
    if (song.empty())
    {
        return 0.0;
    }

    std::array<Token, MAX_ORDER> history{};
    history.fill(START_TOKEN);

    double score = 0.0;
    int num_notes = 0;

    for (const auto& token : song)
    {
        if (token == START_TOKEN)
        {
            continue;
        }

        double probability = markov_trie.get_probability(history, token);

        score += std::log(probability + EPSILON);

        std::shift_left(history.begin(), history.end(), 1);
        history.back() = token;
        num_notes++;
    }

    if (num_notes == 0)
    {
        return 0.0;
    }

    return score / static_cast<double>(num_notes);
}
