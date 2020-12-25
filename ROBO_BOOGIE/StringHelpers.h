#pragma once

#include <string>
#ifdef PI
#include <strings.h>
#endif

inline bool stringEqualIgnoreCase(const std::string& x, const std::string& y)
{
  if (x.size() != y.size())
  {
    return false;
  }

#ifdef PI
    return (strncasecmp(x.data(), y.data(), x.size()) == 0);
#else
    return (_strnicmp(x.data(), y.data(), x.size()) == 0);
#endif
}

