#ifndef ABC_SAVER_HPP
#define ABC_SAVER_HPP

#include "note_saver.hpp"

class AbcSaver : public NoteSaver
{
  public:
    void save(const Notes& notes, std::ostream& stream) override;
};

#endif
