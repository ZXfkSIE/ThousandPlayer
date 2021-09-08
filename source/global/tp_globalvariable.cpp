#include "tp_globalvariable.h"

TP_Config &
TP::Config()
{
    static TP_Config config;
    return config;
}

std::mt19937 &
TP::randomEngine()
{
    static std::random_device randomDevice;
    static std::mt19937 randomGenerator { randomDevice() };
    return randomGenerator;
}
