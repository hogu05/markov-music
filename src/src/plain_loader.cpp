#include "plain_loader.hpp"

#include <istream>
#include <sstream>
#include <string>

#include "types.hpp"

Notes PlainLoader::load(std::istream& stream)
{
    Notes notes;
    Pitch pitch = 0;
    Unit start = 0;
    Unit duration = 0;
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream iss(line);
        if (iss >> pitch >> start >> duration && duration > 0)
        {
            notes.push_back({.start = start, .pitch = pitch, .duration = duration});
        }
    }
    return notes;
}
