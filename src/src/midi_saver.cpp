#include "midi_saver.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

#include "binary_utils.hpp"
#include "midi_constants.hpp"

void MidiSaver::save(const Notes& notes, std::ostream& stream)
{
    if (notes.empty())
    {
        return;
    }

    struct Event
    {
        uint32_t tick;
        uint8_t type;
        uint8_t pitch;
        uint8_t velocity;
        auto operator<=>(const Event&) const = default;
    };

    const double ticks_per_unit =
        midi_constants::DEFAULT_TICKS_PER_QUARTER / static_cast<double>(UNITS_PER_QUARTER);

    std::vector<Event> events;
    events.reserve(notes.size() * 2);
    auto to_tick = [&](int units)
    { return static_cast<uint32_t>(std::round(units * ticks_per_unit)); };

    for (const auto& note : notes)
    {
        auto pitch = static_cast<uint8_t>(std::clamp(note.pitch, 0, MAX_PITCH));
        if (note.duration == 0)
        {
            continue;
        }
        uint32_t start_tick = to_tick(note.start);
        uint32_t end_tick = to_tick(note.start + note.duration);
        events.push_back({.tick = start_tick,
                          .type = midi_constants::NOTE_ON,
                          .pitch = pitch,
                          .velocity = midi_constants::DEFAULT_VELOCITY});
        events.push_back(
            {.tick = end_tick, .type = midi_constants::NOTE_OFF, .pitch = pitch, .velocity = 0});
    }

    std::ranges::sort(events);
    std::ostringstream track_stream;
    uint32_t prev_tick = 0;
    for (const auto& event : events)
    {
        binary_utils::write_variable_length(track_stream, event.tick - prev_tick);
        track_stream.put(static_cast<char>(event.type));
        track_stream.put(static_cast<char>(event.pitch));
        track_stream.put(static_cast<char>(event.velocity));
        prev_tick = event.tick;
    }
    binary_utils::write_variable_length(track_stream, 0);
    track_stream.put(static_cast<char>(midi_constants::META));
    track_stream.put(static_cast<char>(midi_constants::META_END_OF_TRACK));
    track_stream.put('\0');

    std::string track = track_stream.str();

    stream.write("MThd", midi_constants::MTHD_ID_SIZE);
    binary_utils::write_big_endian<uint32_t>(stream, midi_constants::MTHD_HEADER_SIZE);
    binary_utils::write_big_endian<uint16_t>(stream, 0);
    binary_utils::write_big_endian<uint16_t>(stream, 1);
    binary_utils::write_big_endian<uint16_t>(stream, midi_constants::DEFAULT_TICKS_PER_QUARTER);

    stream.write("MTrk", midi_constants::MTRK_ID_SIZE);
    binary_utils::write_big_endian<uint32_t>(stream, track.size());
    stream.write(track.data(), static_cast<std::streamsize>(track.size()));
}
