#ifndef MARKOV_TRIE_HPP
#define MARKOV_TRIE_HPP

#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <span>

#include "types.hpp"

struct TrieNode
{
    std::map<Token, int> counts;
    std::map<Token, std::unique_ptr<TrieNode>> children;

    TrieNode() = default;
    TrieNode(const TrieNode& other);
    TrieNode(TrieNode&&) = default;
    TrieNode& operator=(const TrieNode& other);
    TrieNode& operator=(TrieNode&&) = default;
    TrieNode operator+(const TrieNode& other) const;
    bool operator==(const TrieNode& other) const;
};

class MarkovTrie
{
  public:
    void insert(std::span<const Token> history, Token next_token);
    Token predict(std::span<const Token> history, Pitch current_pitch, std::mt19937& rng) const;
    double get_probability(std::span<const Token> history, Pitch current_pitch,
                           Token next_token) const;
    bool empty() const;
    std::size_t size() const;
    void prune(int threshold);
    MarkovTrie operator+(const MarkovTrie& other) const;
    bool operator==(const MarkovTrie& other) const;
    void save(std::ostream& stream) const;
    void load(std::istream& stream);

  private:
    static int get_weight(int depth);
    static void save_node(const TrieNode& node, std::ostream& stream);
    static TrieNode load_node(std::istream& stream);
    std::unique_ptr<TrieNode> root = std::make_unique<TrieNode>();
    std::vector<const TrieNode*> get_nodes(std::span<const Token> history) const;
};

#endif
