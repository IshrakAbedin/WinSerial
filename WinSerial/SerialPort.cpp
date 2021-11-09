#include "SerialPort.h"

namespace wsr
{

    SerialPort::SerialPort(const std::string &portName, BaudRate baudRate)
        : m_PortName(portName), m_BaudRate(baudRate), m_Connected(false)
    { }

    SerialPort::~SerialPort()
    {
        Disconnect();
    }

    void SerialPort::Connect(bool resetOnConnect)
    {
        if (!m_Connected)
        {
            ClearBuffer();
            //Try to connect to the given port through CreateFile
            m_hSerial = CreateFile(("\\\\.\\" + m_PortName).c_str(),
                                         GENERIC_READ | GENERIC_WRITE,
                                         0,
                                         NULL,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL);

            //Check if the connection was successfull
            if (m_hSerial == INVALID_HANDLE_VALUE)
            {
                //If not success full display an Error
                if (GetLastError() == ERROR_FILE_NOT_FOUND)
                {

                    //TODO: Error Handling
                    std::printf("ERROR: Handle was not attached. Reason: %s not available.\n", m_PortName.c_str());
                }
                else
                {
                    //TODO: Error Handling
                    std::printf("ERROR!!!");
                }
            }
            else
            {
                //If connected we try to set the comm parameters
                DCB dcbSerialParams = {0};

                //Try to get the current
                if (!GetCommState(m_hSerial, &dcbSerialParams))
                {
                    //If impossible, show an error
                    // TODO: Handle serial parameters
                    printf("failed to get current serial parameters!");
                }
                else
                {
                    //Define serial connection parameters for the arduino board
                    dcbSerialParams.BaudRate = (DWORD)m_BaudRate;
                    dcbSerialParams.ByteSize = 8;
                    dcbSerialParams.StopBits = ONESTOPBIT;
                    dcbSerialParams.Parity = NOPARITY;
                    //Setting the DTR to Control_Enable ensures that the Arduino is properly
                    //reset upon establishing a connection
                    dcbSerialParams.fDtrControl = resetOnConnect ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;

                    //Set the parameters and check for their proper application
                    if (!SetCommState(m_hSerial, &dcbSerialParams))
                    {
                        // TODO: Serial param handle exception
                        printf("ALERT: Could not set Serial Port parameters");
                    }
                    else
                    {
                        //If everything went fine we're connected
                        m_Connected = true;
                        // //Flush any remaining characters in the buffers
                        PurgeComm(m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                        // //We wait 2s as the arduino board will be reseting
                        // Sleep(ARDUINO_WAIT_TIME);
                    }
                }
            }
        }
        else
        {
            // TODO: Connection already exists exception
            std::printf("Error: Already Connected, Disconnect first");
        }
    }

    void SerialPort::Disconnect()
    {
        if (m_Connected)
        {
            CloseHandle(m_hSerial);
            ClearBuffer();
            m_Connected = false;
        }
    }

    bool SerialPort::ReadData()
    {
        //Number of bytes we'll have read
        DWORD bytesRead;
        //Number of bytes we'll really ask to read
        unsigned int toRead;

        //Use the ClearCommError function to get status info on the Serial port
        ClearCommError(this->m_hSerial, &this->m_Errors, &this->m_Status);

        // Check if there is something to read
        if (this->m_Status.cbInQue > 0)
        {
            //If there is we check if there is enough data to read the required number
            //of characters, if not we'll read only the available characters to prevent
            //locking of the application.
            if (this->m_Status.cbInQue > BUFFERLEN)
            {
                toRead = BUFFERLEN;
            }
            else
            {
                toRead = this->m_Status.cbInQue;
            }

            //Try to read the require number of chars, and return the number of read bytes on success
            if (ReadFile(this->m_hSerial, m_Buffer, toRead, &bytesRead, NULL))
            {
                this->m_Buffer[bytesRead] = 0;
                return true;
            }
        }

        //If nothing has been read, or that an error was detected return 0
        return false;
    }

    std::string SerialPort::ReadLine()
    {
        ClearBuffer();
        static char readChar;
        static DWORD bytesRead;

        while(true)
        {
            ClearCommError(m_hSerial, &m_Errors, &m_Status);
            if (m_Status.cbInQue > 0)
            {
                if (ReadFile(m_hSerial, &readChar, 1, &bytesRead, NULL))
                {
                    if(readChar == '\n') 
                    {
                        m_Buffer[m_BufferIndex] = 0;
                        return std::string(m_Buffer);
                    }
                    else
                    {
                        m_Buffer[m_BufferIndex++] = readChar;
                    }
                }
            }
        }
    }

    bool SerialPort::WriteData(const std::string &buffer)
    {
        DWORD bytesSend;

        //Try to write the buffer on the Serial port
        if (!WriteFile(this->m_hSerial, (void *)(buffer.c_str()), buffer.length(), &bytesSend, 0))
        {
            //In case it don't work get comm error and return false
            ClearCommError(this->m_hSerial, &this->m_Errors, &this->m_Status);

            return false;
        }
        else
            return true;
    }

    bool SerialPort::IsConnected() const
    {
        //Simply return the connection status
        return this->m_Connected;
    }

}