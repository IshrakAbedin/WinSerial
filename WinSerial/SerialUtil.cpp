#include "SerialUtil.h"

#include <windows.h>

namespace wsr
{
static constexpr size_t BUFFERLEN = 4096; 

std::vector<std::string> GetAvailablePortList()
{
    char lpTargetPath[BUFFERLEN]; // buffer to store the path of the COMPORTS
    std::vector<std::string> portList;

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        std::string portName = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
        DWORD isAvailable = QueryDosDevice(portName.c_str(), lpTargetPath, BUFFERLEN);

        // Test the return value and error if any
        if (isAvailable) //QueryDosDevice returns zero if it didn't find an object
        {
            portList.push_back(portName);
        }

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            // TODO: Add exception here
        }
    }
    return std::move(portList);
}

} // namespace wsr