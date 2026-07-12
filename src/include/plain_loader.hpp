#ifndef PLAIN_LOADER_HPP
#define PLAIN_LOADER_HPP

#include "note_loader.hpp"

class PlainLoader : public NoteLoader
{
  public:
    Notes load(std::istream& stream) override;
};

#endif
