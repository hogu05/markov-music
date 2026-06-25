#include "midi_loader.hpp"

#include <cmath>
#include <map>

#include "binary_utils.hpp"

constexpr int CHUNK_ID_BYTES = 4;
constexpr int MTHD_PREAMBLE_SIZE = 6;

static constexpr uint8_t STATUS_BIT = 0b10000000;
static constexpr uint8_t EVENT_TYPE_MASK = 0b11110000;
static constexpr uint8_t NOTE_OFF = 0b10000000;
static constexpr uint8_t NOTE_ON = 0b10010000;
static constexpr uint8_t AFTERTOUCH = 0b10100000;
static constexpr uint8_t CONTROL_CHANGE = 0b10110000;
static constexpr uint8_t PROGRAM_CHANGE = 0b11000000;
static constexpr uint8_t CHANNEL_PRESSURE = 0b11010000;
static constexpr uint8_t PITCH_BEND = 0b11100000;
static constexpr uint8_t SYSEX = 0b11110000;
static constexpr uint8_t SYSEX_END = 0b11110111;
static constexpr uint8_t META = 0b11111111;
static constexpr uint8_t META_END_OF_TRACK = 0b00101111;

void MidiLoader::read_notes(std::istream& stream, uint16_t ticks_per_quarter, Notes& notes)
{
    std::string id(CHUNK_ID_BYTES, '\0');
    stream.read(id.data(), CHUNK_ID_BYTES);
    if (id != "MTrk")
    {
        return;
    }

    auto length = binary_utils::read_big_endian<uint32_t>(stream);
    auto end_position = stream.tellg() + std::streamoff(length);

    uint32_t tick = 0;
    uint8_t status = 0;
    bool end_of_track = false;
    std::map<uint8_t, std::pair<uint32_t, std::size_t>> active_notes;

    while (stream.tellg() < end_position && !end_of_track)
    {
        tick += binary_utils::read_variable_length(stream);

        uint8_t byte = stream.peek();
        if ((byte & STATUS_BIT) != 0)
        {
            status = stream.get();
        }

        uint8_t event_type = status & EVENT_TYPE_MASK;

        switch (event_type)
        {
        case NOTE_ON:
        case NOTE_OFF:
        {
            uint8_t pitch = stream.get();
            uint8_t velocity = stream.get();
            bool is_note_on = (event_type == NOTE_ON) && (velocity > 0);

            if (is_note_on)
            {
                Unit start = static_cast<Unit>(
                    std::round(static_cast<double>(tick) * UNITS_PER_QUARTER / ticks_per_quarter));
                active_notes[pitch] = {tick, notes.size()};
                notes.push_back({.start = start, .pitch = pitch, .duration = 0});
            }
            else
            {
                auto it = active_notes.find(pitch);
                if (it != active_notes.end())
                {
                    Unit duration =
                        static_cast<Unit>(std::round(static_cast<double>(tick - it->second.first) *
                                                     UNITS_PER_QUARTER / ticks_per_quarter));
                    notes[it->second.second].duration = duration;
                    active_notes.erase(it);
                }
            }
            break;
        }
        case AFTERTOUCH:
        case CONTROL_CHANGE:
        case PITCH_BEND:
            stream.ignore(2);
            break;
        case PROGRAM_CHANGE:
        case CHANNEL_PRESSURE:
            stream.ignore(1);
            break;
        default:
            if (status == META)
            {
                uint8_t meta_type = stream.get();
                uint32_t meta_len = binary_utils::read_variable_length(stream);
                stream.ignore(meta_len);
                if (meta_type == META_END_OF_TRACK)
                {
                    end_of_track = true;
                }
            }
            else if (status == SYSEX || status == SYSEX_END)
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
    std::string id(CHUNK_ID_BYTES, '\0');
    stream.read(id.data(), CHUNK_ID_BYTES);
    if (id != "MThd")
    {
        return {};
    }

    stream.ignore(MTHD_PREAMBLE_SIZE);
    auto num_tracks = binary_utils::read_big_endian<uint16_t>(stream);
    auto ticks_per_quarter = binary_utils::read_big_endian<uint16_t>(stream);

    Notes notes;
    for (int i = 0; i < num_tracks; ++i)
    {
        read_notes(stream, ticks_per_quarter, notes);
    }

    return notes;
}
