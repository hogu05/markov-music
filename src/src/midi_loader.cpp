#include "midi_loader.hpp"

#include <cmath>
#include <map>

#include "binary_utils.hpp"
#include "midi_constants.hpp"

void MidiLoader::read_notes(std::istream& stream, uint16_t ticks_per_quarter, Notes& notes)
{
    std::string id(midi_constants::MTRK_ID_SIZE, '\0');
    stream.read(id.data(), midi_constants::MTRK_ID_SIZE);
    if (id != "MTrk")
    {
        return;
    }

    auto length = binary_utils::read_big_endian<uint32_t>(stream);
    auto end_position = stream.tellg() + std::streamoff(length);

    auto to_unit = [&](uint32_t ticks)
    {
        return static_cast<Unit>(
            std::round(static_cast<double>(ticks) * UNITS_PER_QUARTER / ticks_per_quarter));
    };

    uint32_t tick = 0;
    uint8_t status = 0;
    bool end_of_track = false;
    std::map<uint8_t, std::pair<uint32_t, std::size_t>> active_notes;

    while (stream.tellg() < end_position && !end_of_track)
    {
        tick += binary_utils::read_variable_length(stream);

        uint8_t byte = stream.peek();
        if ((byte & midi_constants::STATUS_BIT) != 0)
        {
            status = stream.get();
        }

        uint8_t event_type = status & midi_constants::EVENT_TYPE_MASK;

        switch (event_type)
        {
        case midi_constants::NOTE_ON:
        case midi_constants::NOTE_OFF:
        {
            uint8_t pitch = stream.get();
            uint8_t velocity = stream.get();
            bool is_note_on = (event_type == midi_constants::NOTE_ON) && (velocity > 0);

            if (is_note_on)
            {
                Unit start = to_unit(tick);
                active_notes[pitch] = {tick, notes.size()};
                notes.push_back({.start = start, .pitch = pitch, .duration = 0});
            }
            else
            {
                auto it = active_notes.find(pitch);
                if (it != active_notes.end())
                {
                    Unit duration = to_unit(tick - it->second.first);
                    notes[it->second.second].duration = duration;
                    active_notes.erase(it);
                }
            }
            break;
        }
        case midi_constants::AFTERTOUCH:
        case midi_constants::CONTROL_CHANGE:
        case midi_constants::PITCH_BEND:
            stream.ignore(2);
            break;
        case midi_constants::PROGRAM_CHANGE:
        case midi_constants::CHANNEL_PRESSURE:
            stream.ignore(1);
            break;
        default:
            if (status == midi_constants::META)
            {
                uint8_t meta_type = stream.get();
                uint32_t meta_len = binary_utils::read_variable_length(stream);
                stream.ignore(meta_len);
                if (meta_type == midi_constants::META_END_OF_TRACK)
                {
                    end_of_track = true;
                }
            }
            else if (status == midi_constants::SYSEX || status == midi_constants::SYSEX_END)
            {
                uint32_t sysex_len = binary_utils::read_variable_length(stream);
                stream.ignore(sysex_len);
            }
            break;
        }
    }
}

Notes MidiLoader::load(std::istream& stream)
{
    std::string id(midi_constants::MTHD_ID_SIZE, '\0');
    stream.read(id.data(), midi_constants::MTHD_ID_SIZE);
    if (id != "MThd")
    {
        return {};
    }

    stream.ignore(midi_constants::MTHD_HEADER_SIZE);
    auto num_tracks = binary_utils::read_big_endian<uint16_t>(stream);
    auto ticks_per_quarter = binary_utils::read_big_endian<uint16_t>(stream);

    Notes notes;
    for (int i = 0; i < num_tracks; ++i)
    {
        read_notes(stream, ticks_per_quarter, notes);
    }

    return notes;
}
