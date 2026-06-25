#ifndef MIDI_CONSTANTS_HPP
#define MIDI_CONSTANTS_HPP

#include <cstdint>

namespace midi_constants
{

inline constexpr uint16_t DEFAULT_TICKS_PER_QUARTER = 480;
inline constexpr uint8_t DEFAULT_VELOCITY = 64;

inline constexpr int MTHD_ID_SIZE = 4;
inline constexpr int MTRK_ID_SIZE = 4;
inline constexpr uint32_t MTHD_HEADER_SIZE = 6;

inline constexpr uint8_t STATUS_BIT = 0b10000000;
inline constexpr uint8_t EVENT_TYPE_MASK = 0b11110000;
inline constexpr uint8_t NOTE_OFF = 0b10000000;
inline constexpr uint8_t NOTE_ON = 0b10010000;
inline constexpr uint8_t AFTERTOUCH = 0b10100000;
inline constexpr uint8_t CONTROL_CHANGE = 0b10110000;
inline constexpr uint8_t PROGRAM_CHANGE = 0b11000000;
inline constexpr uint8_t CHANNEL_PRESSURE = 0b11010000;
inline constexpr uint8_t PITCH_BEND = 0b11100000;
inline constexpr uint8_t SYSEX = 0b11110000;
inline constexpr uint8_t SYSEX_END = 0b11110111;
inline constexpr uint8_t META = 0b11111111;
inline constexpr uint8_t META_END_OF_TRACK = 0b00101111;

} // namespace midi_constants

#endif
