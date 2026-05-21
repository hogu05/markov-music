#include "markov_trie.hpp"

#include <ranges>

void MarkovTrie::insert(std::span<const Token> history, Token next_token)
{
    TrieNode* current_node = root.get();

    auto update_counts = [&](TrieNode* node)
    {
        node->counts[next_token]++;
        node->total_count++;
    };

    update_counts(current_node);

    for (const auto& prev_token : std::ranges::reverse_view(history))
    {
        std::unique_ptr<TrieNode>& child_node_ptr = current_node->children[prev_token];
        if (!child_node_ptr)
        {
            child_node_ptr = std::make_unique<TrieNode>();
        }

        current_node = child_node_ptr.get();
        update_counts(current_node);
    }
}

std::vector<const TrieNode*> MarkovTrie::get_nodes(std::span<const Token> history) const
{
    std::vector<const TrieNode*> nodes = {root.get()};

    for (const auto& prev_token : std::ranges::reverse_view(history))
    {
        auto it = nodes.back()->children.find(prev_token);
        if (it == nodes.back()->children.end())
        {
            break;
        }
        nodes.push_back(it->second.get());
    }
    return nodes;
}

Token MarkovTrie::predict(std::span<const Token> history, Pitch current_pitch,
                          std::mt19937& rng) const
{
    std::vector<const TrieNode*> nodes = get_nodes(history);

    std::vector<std::pair<Token, int>> candidates;

    for (const auto* node : std::ranges::reverse_view(nodes))
    {
        for (const auto& [token, count] : node->counts)
        {
            Pitch pitch = current_pitch + token.pitch_delta;
            if (pitch >= 0 && pitch <= MAX_PITCH)
            {
                candidates.emplace_back(token, count);
            }
        }

        if (!candidates.empty())
        {
            break;
        }
    }

    // TODO: idk
    if (candidates.empty())
    {
        return END_TOKEN;
    }

    int total_count = 0;
    for (const auto& [token, count] : candidates)
    {
        total_count += count;
    }

    std::uniform_int_distribution<int> dist(0, total_count - 1);
    int roll = dist(rng);
    int cumulative = 0;

    for (const auto& candidate : candidates)
    {
        cumulative += candidate.second;
        if (roll < cumulative)
        {
            return candidate.first;
        }
    }
    return candidates.back().first;
}

double MarkovTrie::get_probability(std::span<const Token> history, Pitch current_pitch,
                                   Token next_token) const
{
    std::vector<const TrieNode*> nodes = get_nodes(history);

    double total_score = 0.0;
    double total_weight = 0.0;

    for (int depth = 0; depth < static_cast<int>(nodes.size()); ++depth)
    {
        const auto* node = nodes[depth];
        int weight = depth + 1;
        // TODO: think about the scaling
        // int weight = 1 << depth

        int total_valid_count = 0;
        int next_token_count = 0;

        for (const auto& [token, count] : node->counts)
        {
            Pitch pitch = current_pitch + token.pitch_delta;
            if (pitch >= 0 && pitch <= MAX_PITCH)
            {
                total_valid_count += count;
                if (token == next_token)
                {
                    next_token_count = count;
                }
            }
        }

        if (total_valid_count > 0)
        {
            total_score += weight * (static_cast<double>(next_token_count) / total_valid_count);
            total_weight += weight;
        }
    }

    return total_weight > 0 ? total_score / total_weight : 0.0;
}
