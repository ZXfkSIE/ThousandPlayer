#ifndef TP_GLOBALVARIABLE_H
#define TP_GLOBALVARIABLE_H

#include "tp_config.h"

#include <random>

namespace TP
{
    TP_Config &     Config();
    std::mt19937 &  randomEngine();
}

#endif // TP_GLOBALVARIABLE_H
