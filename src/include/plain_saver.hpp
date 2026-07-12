#ifndef PLAIN_SAVER_HPP
#define PLAIN_SAVER_HPP

#include "note_saver.hpp"

class PlainSaver : public NoteSaver
{
  public:
    void save(const Notes& notes, std::ostream& stream) override;
};

#endif
