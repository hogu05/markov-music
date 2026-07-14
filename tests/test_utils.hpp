#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <iostream>
#include <string>

#include "types.hpp"

static bool check(bool condition, const std::string& name)
{
    std::cout << (condition ? "PASS" : "FAIL") << ": " << name << std::endl;
    return condition;
}

const Notes TEST_NOTES = {
    {.start = 0, .pitch = 109, .duration = 32},   {.start = 6, .pitch = 42, .duration = 8},
    {.start = 9, .pitch = 31, .duration = 16},    {.start = 13, .pitch = 122, .duration = 16},
    {.start = 16, .pitch = 63, .duration = 6},    {.start = 19, .pitch = 59, .duration = 4},
    {.start = 25, .pitch = 56, .duration = 24},   {.start = 29, .pitch = 45, .duration = 6},
    {.start = 35, .pitch = 41, .duration = 32},   {.start = 39, .pitch = 114, .duration = 32},
    {.start = 43, .pitch = 97, .duration = 8},    {.start = 46, .pitch = 39, .duration = 3},
    {.start = 50, .pitch = 103, .duration = 16},  {.start = 52, .pitch = 82, .duration = 1},
    {.start = 60, .pitch = 32, .duration = 2},    {.start = 63, .pitch = 125, .duration = 3},
    {.start = 71, .pitch = 116, .duration = 12},  {.start = 75, .pitch = 55, .duration = 2},
    {.start = 79, .pitch = 57, .duration = 12},   {.start = 83, .pitch = 92, .duration = 16},
    {.start = 87, .pitch = 105, .duration = 6},   {.start = 91, .pitch = 112, .duration = 1},
    {.start = 97, .pitch = 99, .duration = 2},    {.start = 103, .pitch = 53, .duration = 24},
    {.start = 111, .pitch = 117, .duration = 6},  {.start = 119, .pitch = 81, .duration = 8},
    {.start = 121, .pitch = 121, .duration = 6},  {.start = 125, .pitch = 85, .duration = 3},
    {.start = 129, .pitch = 123, .duration = 1},  {.start = 135, .pitch = 28, .duration = 6},
    {.start = 139, .pitch = 48, .duration = 3},   {.start = 143, .pitch = 118, .duration = 2},
    {.start = 151, .pitch = 71, .duration = 6},   {.start = 159, .pitch = 107, .duration = 24},
    {.start = 163, .pitch = 47, .duration = 4},   {.start = 166, .pitch = 110, .duration = 8},
    {.start = 174, .pitch = 95, .duration = 3},   {.start = 178, .pitch = 34, .duration = 24},
    {.start = 180, .pitch = 33, .duration = 32},  {.start = 184, .pitch = 52, .duration = 16},
    {.start = 186, .pitch = 90, .duration = 2},   {.start = 190, .pitch = 50, .duration = 6},
    {.start = 193, .pitch = 96, .duration = 1},   {.start = 196, .pitch = 86, .duration = 32},
    {.start = 198, .pitch = 66, .duration = 2},   {.start = 204, .pitch = 44, .duration = 16},
    {.start = 212, .pitch = 79, .duration = 2},   {.start = 220, .pitch = 30, .duration = 24},
    {.start = 228, .pitch = 74, .duration = 3},   {.start = 231, .pitch = 127, .duration = 16},
    {.start = 235, .pitch = 62, .duration = 3},   {.start = 239, .pitch = 35, .duration = 24},
    {.start = 247, .pitch = 88, .duration = 32},  {.start = 251, .pitch = 89, .duration = 4},
    {.start = 255, .pitch = 94, .duration = 4},   {.start = 261, .pitch = 46, .duration = 6},
    {.start = 265, .pitch = 68, .duration = 8},   {.start = 269, .pitch = 67, .duration = 24},
    {.start = 273, .pitch = 51, .duration = 2},   {.start = 276, .pitch = 64, .duration = 4},
    {.start = 278, .pitch = 40, .duration = 8},   {.start = 280, .pitch = 113, .duration = 32},
    {.start = 284, .pitch = 80, .duration = 4},   {.start = 288, .pitch = 126, .duration = 4},
    {.start = 290, .pitch = 72, .duration = 2},   {.start = 296, .pitch = 102, .duration = 1},
    {.start = 299, .pitch = 91, .duration = 2},   {.start = 301, .pitch = 87, .duration = 8},
    {.start = 303, .pitch = 75, .duration = 24},  {.start = 306, .pitch = 36, .duration = 6},
    {.start = 312, .pitch = 61, .duration = 16},  {.start = 315, .pitch = 115, .duration = 24},
    {.start = 318, .pitch = 54, .duration = 32},  {.start = 322, .pitch = 111, .duration = 16},
    {.start = 325, .pitch = 77, .duration = 16},  {.start = 333, .pitch = 108, .duration = 12},
    {.start = 336, .pitch = 124, .duration = 2},  {.start = 338, .pitch = 60, .duration = 12},
    {.start = 342, .pitch = 49, .duration = 12},  {.start = 346, .pitch = 58, .duration = 16},
    {.start = 354, .pitch = 65, .duration = 1},   {.start = 360, .pitch = 104, .duration = 2},
    {.start = 362, .pitch = 120, .duration = 12}, {.start = 368, .pitch = 76, .duration = 8},
    {.start = 376, .pitch = 73, .duration = 2},   {.start = 379, .pitch = 100, .duration = 4},
    {.start = 382, .pitch = 84, .duration = 24},  {.start = 386, .pitch = 83, .duration = 3},
    {.start = 390, .pitch = 38, .duration = 3},   {.start = 394, .pitch = 101, .duration = 16},
    {.start = 397, .pitch = 106, .duration = 2},  {.start = 401, .pitch = 43, .duration = 24},
    {.start = 403, .pitch = 98, .duration = 1},   {.start = 409, .pitch = 29, .duration = 24},
    {.start = 417, .pitch = 78, .duration = 1},   {.start = 419, .pitch = 93, .duration = 4},
    {.start = 422, .pitch = 37, .duration = 12},  {.start = 426, .pitch = 119, .duration = 16},
    {.start = 429, .pitch = 69, .duration = 12},  {.start = 431, .pitch = 70, .duration = 3},
};

const Track TEST_TRACK = notes_to_track(TEST_NOTES);

#endif
