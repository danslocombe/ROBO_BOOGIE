#pragma once

#include <vector>

struct ASDRConfig
{
    double Attack;
    double Sustain;
    double Decay;
    double Release;

    double ValDown(double input) const
    {
        if (input < Attack)
        {
            return input / Attack;
        }
        else if (input < Attack + Decay)
        {
            const double slope = (Sustain - 1.0) / Decay;
            return slope * (input - Attack) + 1.0;
        }
        
        return Sustain;
    }

    double ValUp(double input)
    {
        if (input < Release)
        {
            return Sustain * (Release - input) / Release;
        }

        return 0.0;
    }
};

inline double lerp(double x0, double x1, double k)
{
    return (x0 * (k - 1) + x1) / k;
}
