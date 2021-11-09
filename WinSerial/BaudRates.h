#pragma once

#include <Windows.h>

namespace wsr
{

enum class BaudRate
{
    //110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000 and 256000
    BR_110 = 110,
    BR_300 = 300,
    BR_600 = 600,
    BR_1200 = 1200,
    BR_2400 = 2400,
    BR_4800 = 4800,
    BR_9600 = 9600,
    BR_14400 = 14400,
    BR_19200 = 19200,
    BR_38400 = 38400,
    BR_56000 = 56000,
    BR_57600 = 57600,
    BR_115200 = 115200,
    BR_128000 = 128000,
    BR_256000 = 256000
};

}