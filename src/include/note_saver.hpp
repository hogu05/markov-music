#ifndef NOTE_SAVER_HPP
#define NOTE_SAVER_HPP

#include <ostream>

#include "types.hpp"

class NoteSaver
{
  public:
    virtual void save(const Notes& notes, std::ostream& stream) = 0;
    virtual ~NoteSaver() = default;
};

#endif
