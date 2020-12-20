#pragma once
#include <string>
#include <cctype>

// Custom implementation for compatibility reasons
inline bool startsWith(const std::string& s, const char* start, const size_t len)
{
  for (size_t i = 0; i < len; i++)
  {
    if (s[i] != start[i])
    {
      return false;
    }
  }

  return true;
}

inline bool endsWith(const std::string& s, const char* end, const size_t len)
{
    const size_t offset = s.size() - len;
    for (size_t i = 0; i < len; i++)
    {
        if (s[offset + i] != end[i])
        {
          return false;
        }
    }

    return true;
}

inline void trimEnd(std::string& s)
{
    while (s.size() > 0)
    {
        const char last = s[s.size() - 1];
        if (isspace(last))
        {
            s.resize(s.size() - 1);
        }
        else
        {
            return;
        }
    }
}
