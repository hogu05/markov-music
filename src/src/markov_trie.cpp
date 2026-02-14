#include "markov_trie.hpp"

#include <ranges>

void MarkovTrie::insert(std::span<const Token> history, Token next_token)
{
    TrieNode* current = root.get();

    auto update_stats = [&](TrieNode* node)
    {
        node->predictions[next_token]++;
        node->total_weight++;
    };

    update_stats(current);

    for (const auto& prev_token : std::ranges::reverse_view(history))
    {
        std::unique_ptr<TrieNode>& child_ptr = current->children[prev_token];
        if (!child_ptr)
        {
            child_ptr = std::make_unique<TrieNode>();
        }

        current = child_ptr.get();
        update_stats(current);
    }
}

Token MarkovTrie::predict(std::span<const Token> history, int current_pitch,
                          std::mt19937& rng) const
{
    const TrieNode* best_node = root.get();
    const TrieNode* current = root.get();

    for (const auto& prev_token : std::ranges::reverse_view(history))
    {
        auto it = current->children.find(prev_token);
        if (it == current->children.end())
        {
            break;
        }

        current = it->second.get();

        for (const auto& [token, count] : current->predictions)
        {
            Pitch pitch = current_pitch + token.pitch_delta;
            if (pitch >= 0 && pitch <= MAX_PITCH)
            {
                best_node = current;
                break;
            }
        }
    }

    std::vector<const std::pair<const Token, int>*> candidates;
    int total_weight = 0;

    for (const auto& prediction : best_node->predictions)
    {
        Pitch pitch = current_pitch + prediction.first.pitch_delta;
        if (pitch >= 0 && pitch <= MAX_PITCH)
        {
            candidates.push_back(&prediction);
            total_weight += prediction.second;
        }
    }

    if (candidates.empty())
    {
        return END_TOKEN;
    }

    std::uniform_int_distribution<int> dist(0, total_weight - 1);
    int roll = dist(rng);
    int cumulative = 0;

    for (const auto* candidate : candidates)
    {
        cumulative += candidate->second;
        if (roll < cumulative)
        {
            return candidate->first;
        }
    }
    return candidates.back()->first;
}

double MarkovTrie::get_probability(std::span<const Token> history, Token next_token) const
{
    const TrieNode* current = root.get();

    for (const auto& prev_token : std::ranges::reverse_view(history))
    {
        auto it = current->children.find(prev_token);
        if (it == current->children.end())
        {
            break;
        }
        current = it->second.get();
    }

    auto it = current->predictions.find(next_token);
    double count = (it != current->predictions.end()) ? it->second : 0.0;

    if (current->total_weight == 0)
    {
        return 0.0;
    }

    return count / static_cast<double>(current->total_weight);
}
