#pragma once
#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <memory>

namespace Engine {
    enum class LogLevel {
        Trace,
        Info,
        Warn,
        Error,
        Fatal
    };

    // Add ToString helper functions
    template<typename T>
    std::string ToString(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    // Special case for bool
    inline std::string ToString(bool value) {
        return value ? "true" : "false";
    }

    // Special case for char*
    inline std::string ToString(const char* value) {
        return std::string(value);
    }

    // Add specialization for unique_ptr
    template<typename T>
    std::string ToString(const std::unique_ptr<T>& ptr) {
        if (ptr) {
            std::stringstream ss;
            ss << "unique_ptr(" << ptr.get() << ")";
            return ss.str();
        }
        return "unique_ptr(nullptr)";
    }

    // Add Window toString support
    inline std::string ToString(const Window& window) {
        std::stringstream ss;
        ss << "Window[" << window.GetWidth() << "x" << window.GetHeight() << "]";
        return ss.str(); 
    }

    class Logger {
    public:
        static Logger& Get() {
            static Logger instance;
            return instance;
        }

        // Simple string concat version
        template<typename... Args>
        void LogConcat(LogLevel level, Args&&... args) {
            std::stringstream ss;
            int dummy[] = { 0, ((ss << std::forward<Args>(args)), 0)... };
            (void)dummy;
            Log(level, ss.str());
        }

        // Format string version
        template<typename... Args>
        void LogFormat(LogLevel level, const char* format, Args... args) {
            Log(level, format);  // For now, just output the format string
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

        // Enhanced LogConcat with variable names
        template<typename T>
        void LogValue(LogLevel level, const char* varName, const T& value) {
            std::stringstream ss;
            ss << varName << " = " << ToString(value);
            Log(level, ss.str());
        }

        // Variadic version for multiple variables
        template<typename... Args>
        void LogValues(LogLevel level, Args&&... args) {
            std::stringstream ss;
            (LogValueImpl(ss, std::forward<Args>(args)), ...);
            Log(level, ss.str());
        }

    private:
        template<typename T>
        void LogValueImpl(std::stringstream& ss, const char* name, const T& value) {
            ss << name << " = " << ToString(value) << ", ";
        }

        Logger() = default;
    };

    #define LOG_TRACE_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Trace, __VA_ARGS__)
    #define LOG_INFO_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Info, __VA_ARGS__)
    #define LOG_WARN_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Warn, __VA_ARGS__)
    #define LOG_ERROR_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Error, __VA_ARGS__)

    #define LOG_TRACE(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Trace, __VA_ARGS__)
    #define LOG_INFO(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Info, __VA_ARGS__)
    #define LOG_WARN(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Warn, __VA_ARGS__)
    #define LOG_ERROR(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Error, __VA_ARGS__)
    #define LOG_FATAL(msg) Engine::Logger::Get().Log(Engine::LogLevel::Fatal, msg)

    // New convenience macros
    #define LOG_VAR(level, name, value) Engine::Logger::Get().LogValue(level, #name, value)
    #define LOG_VARS(level, ...) Engine::Logger::Get().LogValues(level, __VA_ARGS__)
}