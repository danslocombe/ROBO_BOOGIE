#include "RoutineSet.h"
#ifdef PI 
    #include "PiIO.h"
#else
    #include "DevIO.h"
#endif

void RoutineSet::Run()
{
    const auto& routine = m_routines[m_currentRule];
    std::cout << "Running routine: " << routine.Name << std::endl;
    routine.Run();
    m_currentRule = (m_currentRule + 1) % m_routines.size();
}

void Routine::Run() const
{
    for (const auto& move : Moves)
    {
        move.Run();
    }
}

void Move::Run() const
{
    if (std::holds_alternative<Delay>(this->_Move))
    {
        const auto& delay = std::get<Delay>(this->_Move);
        delay.Run();
    }
    else
    {
        const auto& motorMove = std::get<MotorMove>(this->_Move);
        motorMove.Run();
    }
}

void Delay::Run() const
{
    int delayMs = 0;
    if (std::holds_alternative<int>(this->Ms))
    {
        delayMs = std::get<int>(this->Ms);
    }
    else
    {
        const auto range = std::get<RandRange>(this->Ms);
        delayMs = range.Low + rand() % (range.High - range.Low);
    }

    ioDelay(delayMs);
}

void MotorMove::Run() const
{
    const int id = this->Motor == MotorType::Arm ? 0 : 1;
    ioPwmWrite(id, this->Pos);
    const int delayMs = (int)(100.0 + (1.0 - this->Vel) * 200.0);
    ioDelay(delayMs);
}

