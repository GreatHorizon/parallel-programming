#pragma once

#include <string>
#include <stdexcept>
#include <chrono>


unsigned long ConvertDigitStringIntoNumber(const std::string& valueLine)
{
    unsigned long number = 0;
    size_t stoppedAt;
    try
    {
        number = stoul(valueLine, &stoppedAt);
    }
    catch (const std::exception&)
    {
        throw;
    }

    if (valueLine[stoppedAt] != '\0')
    {
        throw std::invalid_argument("Number should not contain letters");
    }

    return number;
}
