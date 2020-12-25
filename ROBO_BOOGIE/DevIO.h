#pragma once
#include <iostream>
#include <Windows.h>
#include "RoutineSet.h"

extern bool switchEnabled;

inline void ioInit()
{
    switchEnabled = false;
}

inline void ioDelay(int x)
{
    //std::cout << "delay: " << x << std::endl;
    Sleep(x);
}

inline bool ioRead()
{
    std::cout << "reading" << std::endl;
    return switchEnabled;
}

inline bool ioReadBlock()
{
    std::cout << "blocking for read" << std::endl;
    char line[100];
    std::cin.getline(line, 100);
    switchEnabled = line[0] == 'y';
    return ioRead();
}

inline void ioPwmWrite(MotorType id, double value)
{
    if (id == MotorType::Arm)
    {
        if (value > 0.8)
        {
            std::cout << "switch turned off" << std::endl;
            switchEnabled = false;
        }
    }

    std::cout << "pwm id: " << MotorTypeString(id) << " val: " << value << std::endl;
}

inline void ioPwmStop(MotorType id)
{
    std::cout << "pwm stop id: " << MotorTypeString(id) << std::endl;
}
