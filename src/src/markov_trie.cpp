#include "markov_trie.hpp"

#include <algorithm>
#include <ranges>

TrieNode::TrieNode(const TrieNode& other) : counts(other.counts)
{
    for (const auto& [token, child] : other.children)
    {
        children[token] = std::make_unique<TrieNode>(*child);
    }
}

TrieNode& TrieNode::operator=(const TrieNode& other)
{
    if (this != &other)
    {
        counts = other.counts;
        children.clear();
        for (const auto& [token, child] : other.children)
        {
            children[token] = std::make_unique<TrieNode>(*child);
        }
    }
    return *this;
}

TrieNode TrieNode::operator+(const TrieNode& other) const
{
    TrieNode result;
    result.counts = counts;
    for (const auto& [token, count] : other.counts)
    {
        result.counts[token] += count;
    }
    for (const auto& [token, child] : children)
    {
        result.children[token] = std::make_unique<TrieNode>(*child);
    }
    for (const auto& [token, child] : other.children)
    {
        auto it = result.children.find(token);
        if (it == result.children.end())
        {
            result.children[token] = std::make_unique<TrieNode>(*child);
        }
        else
        {
            *it->second = *it->second + *child;
        }
    }
    return result;
}

bool TrieNode::operator==(const TrieNode& other) const
{
    return counts == other.counts &&
           std::ranges::equal(children, other.children, [](const auto& a, const auto& b)
                              { return a.first == b.first && *a.second == *b.second; });
}

bool MarkovTrie::operator==(const MarkovTrie& other) const
{
    return *root == *other.root;
}

MarkovTrie MarkovTrie::operator+(const MarkovTrie& other) const
{
    MarkovTrie result;
    *result.root = *root + *other.root;
    return result;
}

bool MarkovTrie::empty() const
{
    return root->counts.empty();
}
void MarkovTrie::insert(std::span<const Token> history, Token next_token)
{
    TrieNode* current_node = root.get();

    auto update_counts = [&](TrieNode* node) { node->counts[next_token]++; };

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

int MarkovTrie::get_weight(int depth)
{
    return depth * depth;
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

    std::map<Token, double> weights;

    for (int depth = 0; depth < static_cast<int>(nodes.size()); ++depth)
    {
        const auto* node = nodes[depth];
        int weight = get_weight(depth + 1);

        int total_valid_count = 0;
        for (const auto& [token, count] : node->counts)
        {
            Pitch pitch = current_pitch + token.pitch_delta;
            if (pitch >= 0 && pitch <= MAX_PITCH)
            {
                total_valid_count += count;
            }
        }

        if (total_valid_count == 0)
        {
            continue;
        }

        for (const auto& [token, count] : node->counts)
        {
            Pitch pitch = current_pitch + token.pitch_delta;
            if (pitch >= 0 && pitch <= MAX_PITCH)
            {
                weights[token] += weight * (static_cast<double>(count) / total_valid_count);
            }
        }
    }

    if (weights.empty())
    {
        return END_TOKEN;
    }

    double total_weight = 0.0;
    for (const auto& [token, w] : weights)
    {
        total_weight += w;
    }

    std::uniform_real_distribution<double> dist(0.0, total_weight);
    double roll = dist(rng);
    double cumulative = 0.0;

    for (const auto& [token, w] : weights)
    {
        cumulative += w;
        if (roll < cumulative)
        {
            return token;
        }
    }
    return weights.rbegin()->first;
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
        int weight = get_weight(depth + 1);

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

void MarkovTrie::save_node(const TrieNode& node, std::ostream& stream)
{
    stream << node.counts.size() << '\n';
    for (const auto& [token, count] : node.counts)
    {
        stream << token.pitch_delta << ' ' << token.duration << ' ' << token.wait << ' ' << count
               << '\n';
    }
    stream << node.children.size() << '\n';
    for (const auto& [token, child] : node.children)
    {
        stream << token.pitch_delta << ' ' << token.duration << ' ' << token.wait << '\n';
        save_node(*child, stream);
    }
}

TrieNode MarkovTrie::load_node(std::istream& stream)
{
    TrieNode node;
    int counts_size = 0;
    stream >> counts_size;
    for (int i = 0; i < counts_size; ++i)
    {
        Token token{};
        int count = 0;
        stream >> token.pitch_delta >> token.duration >> token.wait >> count;
        node.counts[token] = count;
    }
    int children_size = 0;
    stream >> children_size;
    for (int i = 0; i < children_size; ++i)
    {
        Token token{};
        stream >> token.pitch_delta >> token.duration >> token.wait;
        node.children[token] = std::make_unique<TrieNode>(load_node(stream));
    }
    return node;
}

void MarkovTrie::save(std::ostream& stream) const
{
    save_node(*root, stream);
}

void MarkovTrie::load(std::istream& stream)
{
    *root = load_node(stream);
}
