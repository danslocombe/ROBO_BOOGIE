#include "RoutineSet.h"
#include <iostream>
#ifdef PI 
    #include "PiIO.h"
#else
    #include "DevIO.h"
#endif

Routine* RoutineSet::GetRoutineIncrement()
{
    auto* routine = &m_routines[m_currentRule];
    routine->CurrentMove = 0;
    m_currentRule = (m_currentRule + 1) % m_routines.size();
    return routine;
}

bool Routine::Run(SpeechSynthDSP& synth, const ConstantObj& voiceConfig)
{
    if (this->CurrentMove < this->Moves.size())
    {
        const auto& move = this->Moves[this->CurrentMove];
        move.Run(synth, voiceConfig);
        this->CurrentMove++;
        return true;
    }

    return false;
}

void Move::Run(SpeechSynthDSP& synth, const ConstantObj& voiceConfig) const
{
    if (std::holds_alternative<Delay>(this->_Move))
    {
        const auto& delay = std::get<Delay>(this->_Move);
        delay.Run();
    }
    else if (std::holds_alternative<MotorMove>(this->_Move))
    {
        const auto& motorMove = std::get<MotorMove>(this->_Move);
        motorMove.Run();
    }
    else
    {
        const auto& dialogue = std::get<Dialogue>(this->_Move);
        dialogue.Run(synth, voiceConfig);
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
    ioPwmWrite(this->Motor, this->Pos);
    const int delayMs = (int)(750.0 / this->Vel);
    ioDelay(delayMs);
    ioPwmStop(this->Motor);
}

void Dialogue::Run(SpeechSynthDSP& synth, const ConstantObj& config) const
{
    std::cout << "Talking: " << this->Text << ", voice: " << this->Voice << std::endl;
    synth.Talk(this->Text);
    synth.SetSpeaker(this->Voice);
    while (synth.IsTalking())
    {
        synth.Tick(config);
        ioDelay(2);
    }
}

