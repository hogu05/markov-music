#ifndef MIDI_PROCESSOR_HPP
#define MIDI_PROCESSOR_HPP

#include <string>

#include "types.hpp"

namespace midi_processor
{
Track load_track(const std::string& filename);
void save_track(const Track& track, const std::string& filename);
} // namespace midi_processor

#endif
