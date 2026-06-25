#ifndef MIDI_SAVER_HPP
#define MIDI_SAVER_HPP

#include "note_saver.hpp"

class MidiSaver : public NoteSaver
{
  public:
    void save(const Notes& notes, std::ostream& stream) override;

};

#endif
