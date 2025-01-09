#pragma once
#include <string>
#include <iostream>
#include <ctime>
#include <sstream>

namespace Engine {
    enum class LogLevel {
        Trace,
        Info,
        Warn,
        Error,
        Fatal
    };

    class Logger {
    public:
        static Logger& Get() {
            static Logger instance;
            return instance;
        }

        void Log(LogLevel level, const std::string& message) {
            std::time_t now = std::time(nullptr);
            char buffer[32];
            std::strftime(buffer, 32, "%c", std::localtime(&now));
            std::string timestamp(buffer);

            std::string levelStr;
            switch (level) {
                case LogLevel::Trace: levelStr = "TRACE"; break;
                case LogLevel::Info:  levelStr = "INFO"; break;
                case LogLevel::Warn:  levelStr = "WARN"; break;
                case LogLevel::Error: levelStr = "ERROR"; break;
                case LogLevel::Fatal: levelStr = "FATAL"; break;
            }

            std::cout << "[" << timestamp << "] [" << levelStr << "]: " << message << std::endl;
        }

    private:
        Logger() = default;
    };

    #define LOG_TRACE(msg) Engine::Logger::Get().Log(Engine::LogLevel::Trace, msg)
    #define LOG_INFO(msg)  Engine::Logger::Get().Log(Engine::LogLevel::Info, msg)
    #define LOG_WARN(msg)  Engine::Logger::Get().Log(Engine::LogLevel::Warn, msg)
    #define LOG_ERROR(msg) Engine::Logger::Get().Log(Engine::LogLevel::Error, msg)
    #define LOG_FATAL(msg) Engine::Logger::Get().Log(Engine::LogLevel::Fatal, msg)
}