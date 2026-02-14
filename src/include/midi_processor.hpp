#ifndef MIDI_PROCESSOR_H
#define MIDI_PROCESSOR_H

#include <string>
#include <vector>

#include "types.hpp"

namespace midi_processor
{
std::vector<Token> load_tokens(const std::string& filename);
void save_tokens(const std::vector<Token>& tokens, const std::string& filename);
} // namespace midi_processor

#endif
