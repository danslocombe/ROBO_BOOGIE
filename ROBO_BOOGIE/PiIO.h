#pragma once
#include <wiringPi.h>
#include <iostream>

constexpr int pinSwitch = 0;
constexpr int pinArm = 1;
constexpr int pinLid = 23;

inline void ioInit()
{
    wiringPiSetup();
    pinMode(pinSwitch, INPUT);
    pinMode(pinArm, PWM_OUTPUT);
    pinMode(pinLid, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(192);
    pwmSetRange(2000);
}

inline void ioDelay(int x)
{
    std::cout << "delay: " << x << std::endl;
    delay(x);
}

inline bool ioRead()
{
    delay(50);
    return digitalRead(pinSwitch) != 0;
}

inline void ioPwmWrite(int id, double value)
{
    std::cout << "pwm id: " << pin << " val: " << value << std::endl;
    const int pin = (id == 0) ? pinArm : pinLid;

    constexpr double min = 70.0;
    constexpr double max = 220.0;
    constexpr double range = max - min;

    const int pwmValue = (int)(min + range * value);
    pwmWrite(pin, value);
}
