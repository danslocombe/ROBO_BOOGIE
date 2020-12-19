#pragma
#include <vector>
#include <string>
#include <variant>

struct Delay
{
	int Ms;
};

enum MotorType
{
	Lid,
	Arm,
};

struct MotorMove
{
	MotorType Motor;
	double Pos;
};

struct Move
{
	std::variant<Delay, MotorMove> _Move;
};


class Moveset
{
	int m_currentRule = 0;
public:
	void Run() const;
};

class MovesetParser
{
public:
	Moveset ParseFile(const std::vector<std::string>& lines);
};