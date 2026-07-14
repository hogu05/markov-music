#ifndef MUSIC_MODEL_HPP
#define MUSIC_MODEL_HPP

#include <istream>
#include <ostream>
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
    bool empty() const;
    MusicModel operator+(const MusicModel& other) const;
    bool operator==(const MusicModel& other) const;
    void save(std::ostream& stream) const;
    void load(std::istream& stream);

  private:
    MarkovTrie markov_trie;
};

#endif
