#pragma once
#include <wiringPi.h>
#include <iostream>

inline void ioInit()
{
    wiringPiSetup();
    pinMode(0, INPUT);
}

inline void ioDelay(int x)
{
    std::cout << "delay: " << x << std::endl;
    delay(x);
}

inline bool ioRead()
{
    delay(50);
    return digitalRead(0) != 0;
}

inline void ioPwmWrite(int id, double value)
{
    std::cout << "pwm id: " << id << " val: " << value << std::endl;
}
