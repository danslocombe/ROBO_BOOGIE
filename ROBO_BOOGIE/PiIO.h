#pragma once
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>

constexpr int pinSwitch = 0;
constexpr int pinArm = 1;
constexpr int pinLid = 23;

inline void ioDelay(int x)
{
    std::cout << "delay: " << x << std::endl;
    delay(x);
}

inline bool ioRead()
{
    std::cout << "IORead" << std::endl;
    char line[100];
    std::cin.getline(line, 100);
    return line[0] == 'y';
    /*
    delay(50);
    return digitalRead(pinSwitch) == 0;
    */
}

inline int getPin(int id)
{
    if (id == 0)
    {
      return pinArm;
    }
    else
    {
      return pinLid;
    }
}

inline void ioPwmWrite(int id, double value)
{
    constexpr double max_lid = 5.0;
    constexpr double min_lid = 22.0;

    constexpr double max_arm = 5.0;
    constexpr double min_arm = 18.0;
    
    const int pin = getPin(id);
    double min = 0.0;
    double max = 0.0;

    if (id == 0)
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

    //std::cout << "pwm id: " << pin << " val: " << pwmValue << std::endl;
    //pwmWrite(pin, pwmValue);
}

inline void ioPwmStop(int id)
{
  const int pin = getPin(id);
  softPwmStop(pin);
}

inline void ioInit()
{
    wiringPiSetup();
    pinMode(pinSwitch, INPUT);

    pinMode(pinArm, OUTPUT);
    pinMode(pinLid, OUTPUT);
    ioPwmWrite(0, 0.0);
    ioPwmStop(0);
    ioPwmWrite(1, 0.0);
    ioPwmStop(1);
/*
    digitalWrite(pinArm, LOW);
    softPwmCreate(pinArm, 0, 200);
    softPwmWrite(pinArm, 15);

    digitalWrite(pinLid, LOW);
    softPwmCreate(pinLid, 0, 200);
    softPwmWrite(pinLid, 15);
    */

    //pinMode(pinArm, PWM_OUTPUT);
    //pinMode(pinLid, PWM_OUTPUT);
    //pwmSetMode(PWM_MODE_MS);
    //pwmSetClock(192);
    //pwmSetRange(2000);

    std::cout << "WiringPi Setup" << std::endl;
}
