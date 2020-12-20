#pragma once
#include <iostream>

inline void ioInit()
{
}

inline void ioDelay(int x)
{
    std::cout << "delay: " << x << std::endl;
}

inline bool ioRead()
{
    char line[100];
    std::cin.getline(line, 100);

    return line[0] == 'y';
}

inline void ioPwmWrite(int id, double value)
{
    std::cout << "pwm id: " << id << " val: " << value << std::endl;
}
