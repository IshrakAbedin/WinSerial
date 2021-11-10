#include <iostream>
#include "WinSerial/SerialUtil.h"
#include "WinSerial/SerialPort.h"

#define DO_ASYNC 1

void readLineCallback(const std::string& portName, std::string msg);

int main()
{
    auto portList = wsr::GetAvailablePortList();
    for (auto &portName : portList)
    {
        std::cout << portName << std::endl;
    }

    if (portList.size() > 0)
    {
        std::cout << "Running Serial connection at: " << portList[0] << std::endl;

        wsr::SerialPort SP{portList[0], wsr::BaudRate::BR_9600};

        SP.Connect(true);

#if !DO_ASYNC
        while(true)
        {
            std::cout << "[Line]: " << SP.ReadLine() << std::endl;
        }
#else
        SP.InitializeAsyncLineReading(readLineCallback);
        Sleep(5000);
        SP.StopAsyncLineReading();
#endif

        SP.Disconnect();
    }
    else
    {
        std::cout << "No COM device found" << std::endl;
    }

    return 0;
}

void readLineCallback(const std::string& portName, std::string msg)
{
    std::cout << "[" << portName <<"]: " << msg << std::endl;
}