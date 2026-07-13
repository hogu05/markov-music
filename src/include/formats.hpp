#ifndef FORMATS_HPP
#define FORMATS_HPP

#include <ios>
#include <optional>
#include <string>

#include "note_loader.hpp"
#include "note_saver.hpp"

namespace formats
{

enum class Format : std::uint8_t
{
    Midi,
    Abc,
    Plain,
};

NoteLoader* get_loader(Format format);
NoteSaver* get_saver(Format format);
std::optional<Format> format_from_extension(const std::string& extension);
std::optional<Format> format_from_string(const std::string& string);
std::ios::openmode open_flags(Format format);

} // namespace formats

#endif
