#include "RoutineSet.h"
#include <iostream>
#include <cstring>

struct Span
{
    size_t Start;
    size_t End;
};

// Custom implementation for compatibility reasons
bool startsWith(const std::string& s, const char* start, const size_t len)
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

bool endsWith(const std::string& s, const char* end, const size_t len)
{
  for (size_t i = s.size() - len; i < s.size(); i++)
  {
    if (s[i] != end[i])
    {
      return false;
    }
  }

  return true;
}

bool eq(const std::string& s, const char* cstring, const size_t len)
{
    if (s.size() != len)
    {
        return false;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (s[i] != cstring[i])
        {
            return false;
        }
    }

    return true;
}

std::vector<Span> SplitSpaceSpans(const std::string& s)
{
    size_t start = 0;
    size_t off = 0;
    std::vector<Span> spans;
    do
    {
        off = s.find_first_of(" (", off + 1);
        if (off != std::string::npos && s[off] == '(')
        {
            // Go to end parens
            off = s.find(")", off + 1);
            // Then carry on
            off = s.find(" ", off + 1);
        }

        spans.emplace_back(Span{start, off});
        start = off + 1;
    } while (off != std::string::npos);

    return spans;
}

std::vector<std::string> SplitSpace(const std::string& s)
{
    const auto spans = SplitSpaceSpans(s);
    std::vector<std::string> splits;
    for (const auto& span : spans)
    {
        const auto substr = s.substr(span.Start, span.End - span.Start);
        splits.emplace_back(substr);
    }

    return splits;
}

std::optional<int> ParseDur(std::string durStr)
{
    double mult = 1.0;
    if (endsWith(durStr, "ms", 2))
    {
        // Keep mult at 1
        durStr.resize(durStr.size() - 2);
    }
    else if (endsWith(durStr, "s", 1))
    {
        mult = 1000.0;
        durStr.resize(durStr.size() - 1);
    }

    const double delay = mult * atof(durStr.c_str());

    if (delay == 0.0)
    {
        std::cout << "Could not parse duration: '" << durStr << "'";
        return std::nullopt;
    }

    return { (int)delay };
}

std::optional<RandRange> ParseRandRange(std::string rangeStr)
{
    // Expect input of the form (500ms, 1s)

    const auto noParens = rangeStr.substr(1, rangeStr.size() - 2);
    const auto commaPos = noParens.find(',');

    if (commaPos == std::string::npos)
    {
        std::cout << "Could not parse random range: '" << rangeStr << "'" << std::endl;
        return std::nullopt;
    }

    const auto first = ParseDur(noParens.substr(0, commaPos));
    if (!first.has_value())
    {
        return std::nullopt;
    }

    const size_t start = rangeStr.find_first_not_of(" ", commaPos + 1);
    const auto second = ParseDur(noParens.substr(start, noParens.size() - start));
    if (!second.has_value())
    {
        return std::nullopt;
    }

    if (second.value() <= first.value())
    {
        std::cout << "Range must be of the form (low, high). got '" << rangeStr << "'" << std::endl;
        return std::nullopt;
    }

    return { RandRange{ first.value(), second.value() }};
}

std::optional<Move> ParseDelay(const std::vector<std::string>& splits)
{
    if (splits.size() > 1)
    {
        const auto& durStr = splits[1];
        if (durStr[0] == '(')
        {
            const auto randRange = ParseRandRange(durStr);
            if (!randRange.has_value())
            {
                return std::nullopt;
            }

            return { Move{ Delay{randRange.value()} } };
        }
        else
        {
            const auto ms = ParseDur(durStr);
            if (!ms.has_value())
            {
                return std::nullopt;
            }

            return { Move{ Delay{ms.value()} } };
        }
    }
    else
    {
        return { Move{ Delay{1000 } } };
    }
}

std::optional<Move> ParseMove(const std::string& line)
{
    if (line.size() == 0 || line[0] == '#')
    {
        // Skip empty or commented
        return std::nullopt;
    }

    const auto splits = SplitSpace(line);
    if (splits.size() == 0)
    {
        // All spaces
        return std::nullopt;
    }

    constexpr const char* LID("lid");
    constexpr const char* DELAY("delay");
    constexpr const char* ARM("arm");

    const auto& command = splits[0];
    if (eq(command, DELAY, strlen(DELAY)))
    {
        return ParseDelay(splits);
    }
    else if (eq(command, LID, strlen(LID)) || eq(command, ARM, strlen(LID)))
    {
        const auto motor = (command == LID)
            ? MotorType::Lid
            : MotorType::Arm;

        double pos = 0.0;
        double vel = 0.5;

        if (splits.size() > 1)
        {
            pos = atof(splits[1].c_str());
        }
        
        if (splits.size() > 2)
        {
            const auto& velStr = splits[2];
            if (eq(velStr, "slow", 4))
            {
                vel = 0.25;
            }
            else if (eq(velStr, "med", 3))
            {
                vel = 0.5;
            }
            else if (eq(velStr, "fast", 4))
            {
                vel = 1.0;
            }
            else
            {
                vel = atof(splits[2].c_str());
            }

            if (vel == 0.0)
            {
                std::cout << "Could not parse " << velStr << " in '" << line << "'" << std::endl;
                return std::nullopt;
            }
        }

        MotorMove move;
        move.Motor = motor;
        move.Pos = pos;
        move.Vel = vel;
        return { Move { std::move(move) } };
    }

    std::cout << "Could not parse line '"  << line << "'" << std::endl;
    return std::nullopt;
}

Routine ParseRoutine(std::string name, const std::vector<std::string>& lines, int start, int end)
{
    Routine routine;
    routine.Name = name;
    for (int i = start; i <= end; i++)
    {
        const auto move = ParseMove(lines[i]);
        if (move.has_value())
        {
            routine.Moves.push_back(move.value());
        }
    }
    return routine;
}

RoutineSet RoutineSetParser::ParseFile(const std::vector<std::string>& lines)
{
    std::vector<Routine> routines;
    std::string curName;
    int start = 0;

    for (int i = 0; i < lines.size(); i++)
    {
        const auto& line = lines[i];
        constexpr const char *ROUTINE("routine");
        if (startsWith(line, ROUTINE, strlen(ROUTINE)))
        {
            if (i > 0)
            {
                auto routine = ParseRoutine(curName, lines, start + 1, i-1);
                routines.emplace_back(std::move(routine));
            }

            curName = line.substr(strlen(ROUTINE) + 1);
            start = i;
        }
    }
    
    auto finalRoutine = ParseRoutine(curName, lines, start + 1, lines.size() - 1);
    routines.emplace_back(std::move(finalRoutine));

    return RoutineSet(routines);
}
