#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include "ConstantObj.h"
#include "SpeechSynth.h"

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

struct Dialogue
{
    std::string Text;
    std::string Voice;
    void Run(SpeechSynthDSP& synth, const ConstantObj& config) const;
};

struct Move
{
    std::variant<Delay, MotorMove, Dialogue> _Move;
    void Run(SpeechSynthDSP& synth, const ConstantObj& config) const;
};

struct Routine
{
    std::string Name;
    std::vector<Move> Moves;
    int CurrentMove = 0;

    bool Run(SpeechSynthDSP& synth, const ConstantObj& config);
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
