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
};

enum class MotorType
{
	Lid,
	Arm,
};

struct MotorMove
{
	MotorType Motor;
	double Pos;
	double Vel;
};

struct Move
{
	std::variant<Delay, MotorMove> _Move;
};

struct Routine
{
	std::string Name;
	std::vector<Move> Moves;
};

class RoutineSet
{
	int m_currentRule = 0;
	std::vector<Routine> m_routines;

public:
	RoutineSet(std::vector<Routine> routines) : m_routines(std::move(routines))
	{}

	void Run() const;
};

class RoutineSetParser
{
public:
	RoutineSet ParseFile(const std::vector<std::string>& lines);
};
