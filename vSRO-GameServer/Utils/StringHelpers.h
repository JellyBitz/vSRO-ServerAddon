#pragma once
#include <string>

// Replace a string from all occurrences found into the value
static void StringReplaceAll(std::string& Value, const std::string& From, const std::string& To)
{
    if (From.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = Value.find(From, start_pos)) != std::string::npos)
    {
        Value.replace(start_pos, From.length(), To);
        start_pos += To.length(); // Increase the same length in case 'To' contains 'From', like replacing 'x' with 'yx'
    }
}