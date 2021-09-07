#include "tp_globalvariable.h"

TP_Config &
TP::Config()
{
    static TP_Config config;
    return config;
}
