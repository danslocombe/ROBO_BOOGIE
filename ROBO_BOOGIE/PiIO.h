#pragma once
#include <wiringPi.h>

inline void ioInit()
{
    wiringPiSetup();
	pinMode(0, INPUT);
}

inline void ioDelay(int x)
{
      delay(50);
}

inline bool ioRead()
{
      return digitalRead(0) != 0;
}

inline void ioPwmWrite(int id, double value)
{
}
