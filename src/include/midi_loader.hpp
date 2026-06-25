#ifndef MIDI_LOADER_HPP
#define MIDI_LOADER_HPP

#include <cstdint>

#include "note_loader.hpp"

class MidiLoader : public NoteLoader
{
  public:
    Notes load(std::istream& stream) override;

  private:
    static void read_notes(std::istream& stream, uint16_t ticks_per_quarter, Notes& notes);
};

#endif
