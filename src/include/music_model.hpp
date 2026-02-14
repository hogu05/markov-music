#ifndef MUSIC_MODEL_H
#define MUSIC_MODEL_H

#include <random>
#include <vector>

#include "markov_trie.hpp"
#include "types.hpp"

class MusicModel
{
  public:
    static constexpr int MAX_ORDER = 5;
    void train(const std::vector<Token>& tokens);
    std::vector<Token> generate(std::mt19937& rng) const;
    double evaluate(const std::vector<Token>& song) const;

  private:
    MarkovTrie markov_trie;
};

#endif
