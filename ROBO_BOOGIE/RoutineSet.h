#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

struct RandRange
{
    int Low;
    int High;
};

struct Delay
{
    std::variant<int, RandRange> Ms;

    void Run() const;
};

enum class MotorType
{
    Lid,
    Arm,
};

inline const char* MotorTypeString(MotorType id)
{
    if (id == MotorType::Lid)
    {
        return "LID";
    }
    else
    {
        return "ARM";
    }
}

struct MotorMove
{
    MotorType Motor;
    double Pos;
    double Vel;

    void Run() const;
};

struct Move
{
    std::variant<Delay, MotorMove> _Move;
    void Run() const;
};

struct Routine
{
    std::string Name;
    std::vector<Move> Moves;
    int CurrentMove = 0;

    bool Run();
};

class RoutineSet
{
    int m_currentRule = 0;
    std::vector<Routine> m_routines;

public:
    RoutineSet(std::vector<Routine> routines) : m_routines(std::move(routines))
    {}

    Routine* GetRoutineIncrement();
};

class RoutineSetParser
{
public:
    RoutineSet ParseFile(const std::vector<std::string>& lines);
};
