#pragma once

#include <string>

inline bool stringEqualIgnoreCase(const std::string& x, const std::string& y)
{
    return (x.size() == y.size()) && (_strnicmp(x.data(), y.data(), x.size()) == 0);
}

