#include "formats.hpp"

#include "abc_saver.hpp"
#include "midi_loader.hpp"
#include "midi_saver.hpp"
#include "plain_loader.hpp"
#include "plain_saver.hpp"

namespace formats
{

NoteLoader* get_loader(Format format)
{
    static MidiLoader midi_loader;
    static PlainLoader plain_loader;
    switch (format)
    {
    case Format::Midi:
        return &midi_loader;
    case Format::Plain:
        return &plain_loader;
    default:
        return nullptr;
    }
}

NoteSaver* get_saver(Format format)
{
    static MidiSaver midi_saver;
    static AbcSaver abc_saver;
    static PlainSaver plain_saver;
    switch (format)
    {
    case Format::Midi:
        return &midi_saver;
    case Format::Abc:
        return &abc_saver;
    case Format::Plain:
        return &plain_saver;
    }
}

std::ios::openmode open_flags(Format format)
{
    return format == Format::Midi ? std::ios::binary : std::ios::in;
}

std::optional<Format> format_from_extension(const std::string& ext)
{
    if (ext == ".mid" || ext == ".midi")
    {
        return Format::Midi;
    }
    if (ext == ".abc")
    {
        return Format::Abc;
    }
    if (ext == ".notes")
    {
        return Format::Plain;
    }
    return std::nullopt;
}

} // namespace formats
