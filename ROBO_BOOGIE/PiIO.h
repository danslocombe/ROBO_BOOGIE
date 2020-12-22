#pragma once
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include "RoutineSet.h"

constexpr int pinSwitch = 0;
constexpr int pinArm = 1;
constexpr int pinLid = 23;

extern bool switchEnabled;

inline void ioDelay(int x)
{
    std::cout << "delay: " << x << std::endl;
    delay(x);
}

inline bool ioRead()
{
    /*
    delay(50);
    return digitalRead(pinSwitch) == 0;
    */
    std::cout << "reading" << std::endl;
    return switchEnabled;
}

inline bool ioReadBlock()
{
    // While fixing actual switch we are gonna lean on std::in 
    std::cout << "blocking for read" << std::endl;
    char line[100];
    std::cin.getline(line, 100);
    switchEnabled = line[0] == 'y';
    return ioRead();
}

inline int getPin(MotorType id)
{
    if (id == MotorType::Arm)
    {
      return pinArm;
    }
    else
    {
      return pinLid;
    }
}

inline void ioPwmWrite(MotorType id, double value)
{
    constexpr double max_lid = 5.0;
    constexpr double min_lid = 22.0;

    constexpr double max_arm = 5.0;
    constexpr double min_arm = 18.0;
    
    const int pin = getPin(id);
    double min = 0.0;
    double max = 0.0;

    if (id == MotorType::Arm)
    {
      min = min_arm;
      max = max_arm;
    }
    else
    {
      min = min_lid;
      max = max_lid;
    }

    const double range = max - min;
    const int pwmValue = (int)(min + range * value);
    softPwmCreate(pin, 0, 200);
    softPwmWrite(pin, pwmValue);
}

inline void ioPwmStop(MotorType id)
{
  const int pin = getPin(id);
  softPwmStop(pin);
}

inline void ioInit()
{
    switchEnabled = false;
    wiringPiSetup();
    pinMode(pinSwitch, INPUT);

    pinMode(pinArm, OUTPUT);
    pinMode(pinLid, OUTPUT);
    ioPwmWrite(0, 0.0);
    ioPwmStop(0);
    ioPwmWrite(1, 0.0);
    ioPwmStop(1);

    std::cout << "WiringPi Setup" << std::endl;
}
