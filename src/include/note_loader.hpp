#ifndef NOTE_LOADER_HPP
#define NOTE_LOADER_HPP

#include <istream>

#include "types.hpp"

class NoteLoader
{
  public:
    virtual Notes load(std::istream& stream) = 0;
    virtual ~NoteLoader() = default;
};

#endif
