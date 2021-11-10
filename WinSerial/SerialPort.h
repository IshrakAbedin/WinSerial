#pragma once

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <string>
#include <functional>
#include <future>

#include "BaudRates.h"

namespace wsr
{
    using rlcallback_t = std::function<void(const std::string&, std::string)>;

    class SerialPort
    {
    private:
        std::string m_PortName;
        BaudRate m_BaudRate;

        HANDLE m_hSerial;       //Serial comm handler
        COMSTAT m_Status;       //COMPORT status
        DWORD m_Errors;         //Error track
        bool m_Connected;       //Connection status

        static constexpr size_t BUFFERLEN = 4096;   // Buffer length
        char m_Buffer[BUFFERLEN];                   // Buffer for storing read data
        size_t m_BufferIndex;                       // Track of buffer position

        bool m_AsyncRunning;                    // Async running status
        rlcallback_t m_AsyncReadingCallback;    // Async callback after reading line
        std::future<void> m_SharedState;        // State of async task

    public:
        SerialPort() = delete;
        SerialPort(SerialPort &other) = delete;
        SerialPort(const std::string &portName, BaudRate baudRate);
        ~SerialPort();
        
        SerialPort &operator=(SerialPort &other) = delete;

        inline std::string GetPortName() const { return m_PortName; }
        inline void SetPortName(const std::string& portName) { m_PortName = portName; }
        inline BaudRate GetBaudRate() const { return m_BaudRate; }
        inline void SetBaudRate(BaudRate baudRate) { m_BaudRate = baudRate; }

        void Connect(bool resetOnConnect = false);
        void Disconnect();
        inline bool IsConnected() const { return m_Connected; };

        bool ReadData();
        inline std::string GetBuffer() const { return std::string(m_Buffer); }
        std::string ReadLine();
        bool WriteData (const std::string &buffer);

        void InitializeAsyncLineReading(rlcallback_t callback);
        void StopAsyncLineReading();
        inline bool IsAsyncReadingRunning() const { return m_AsyncRunning; }

    private:
        inline void ClearBuffer() { m_BufferIndex = 0; m_Buffer[0] = 0; }
        void AsyncLineReadLoop(rlcallback_t callback);
    };

}