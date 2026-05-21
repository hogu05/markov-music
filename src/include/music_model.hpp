#ifndef MUSIC_MODEL_H
#define MUSIC_MODEL_H

#include <random>

#include "markov_trie.hpp"
#include "types.hpp"

class MusicModel
{
  public:
    static constexpr int MAX_ORDER = 5;
    void train(const Track& track);
    Track generate_track(std::mt19937& rng) const;
    double evaluate(const Track& track) const;

  private:
    MarkovTrie markov_trie;
};

#endif
