#pragma once

#include <pch.h>

namespace Engine {
class Window;
}

namespace Engine {
/**
 * @brief Logging severity levels
 */
enum class LogLevel {
    Trace,  ///< Detailed debugging information
    Info,   ///< General information messages
    Warn,   ///< Warning messages
    Error,  ///< Error messages
    Fatal   ///< Critical errors that require immediate attention
};

// Add ToString helper functions
/**
 * @brief Converts any type to its string representation
 * @tparam T Type to convert
 * @param value Value to convert
 * @return String representation
 */
template <typename T>
std::string ToString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Special case for bool
inline std::string ToString(bool value) { return value ? "true" : "false"; }

// Special case for char*
inline std::string ToString(const char* value) { return std::string(value); }

// Add specialization for unique_ptr
template <typename T>
std::string ToString(const std::unique_ptr<T>& ptr) {
    if (ptr) {
        std::stringstream ss;
        ss << "unique_ptr(" << ptr.get() << ")";
        return ss.str();
    }
    return "unique_ptr(nullptr)";
}

inline std::string ToString(const Window& window);

#ifdef _WIN32
/**
 * @brief Singleton logger class for application-wide logging
 */
class Logger {
   public:
    /** @return Reference to the logger instance */
    static Logger& Get() {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Logs a concatenated message
     * @tparam Args Types of arguments to concatenate
     * @param level Severity level of the message
     * @param args Arguments to concatenate into message
     */
    template <typename... Args>
    void LogConcat(LogLevel level, Args&&... args) {
        std::stringstream ss;
        int dummy[] = {0, ((ss << std::forward<Args>(args)), 0)...};
        (void)dummy;
        Log(level, ss.str());
    }

    /**
     * @brief Logs a formatted message
     * @tparam Args Types of format arguments
     * @param level Severity level of the message
     * @param format Format string
     * @param args Format arguments
     */
    template <typename... Args>
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
            case LogLevel::Trace:
                levelStr = "TRACE";
                break;
            case LogLevel::Info:
                levelStr = "INFO";
                break;
            case LogLevel::Warn:
                levelStr = "WARN";
                break;
            case LogLevel::Error:
                levelStr = "ERROR";
                break;
            case LogLevel::Fatal:
                levelStr = "FATAL";
                break;
        }

        std::cout << "[" << timestamp << "] [" << levelStr << "]: " << message << std::endl;
    }

    /**
     * @brief Logs a variable's name and value
     * @tparam T Type of the variable
     * @param level Severity level of the message
     * @param varName Name of the variable
     * @param value Value of the variable
     */
    template <typename T>
    void LogValue(LogLevel level, const char* varName, const T& value) {
        std::stringstream ss;
        ss << varName << " = " << ToString(value);
        Log(level, ss.str());
    }

    /**
     * @brief Logs multiple variables' names and values
     * @tparam Args Types of the variables
     * @param level Severity level of the message
     * @param args Pairs of variable names and values
     */
    template <typename... Args>
    void LogValues(LogLevel level, Args&&... args) {
        std::stringstream ss;
        (LogValueImpl(ss, std::forward<Args>(args)), ...);
        Log(level, ss.str());
    }

   private:
    template <typename T>
    void LogValueImpl(std::stringstream& ss, const char* name, const T& value) {
        ss << name << " = " << ToString(value) << ", ";
    }

    Logger() = default;
};

#define LOG_TRACE_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Trace, __VA_ARGS__)
#define LOG_INFO_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARN_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Fatal, __VA_ARGS__)

#define LOG_TRACE(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Trace, __VA_ARGS__)
#define LOG_INFO(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Fatal, __VA_ARGS__)

// New convenience macros
#define LOG_VAR(level, name, value) Engine::Logger::Get().LogValue(level, #name, value)
#define LOG_VARS(level, ...) Engine::Logger::Get().LogValues(level, __VA_ARGS__)

#else
namespace LogColors {
constexpr const char* Reset = "\033[0m";
constexpr const char* Yellow = "\033[33m";
constexpr const char* Red = "\033[31m";
constexpr const char* Purple = "\033[35m";

#ifdef _WIN32
// Enable ANSI support on Windows
inline void EnableColors() {
    static bool enabled = false;
    if (!enabled) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        enabled = true;
    }
}
#endif
}  // namespace LogColors

/**
 * @brief Singleton logger class for application-wide logging
 */
class Logger {
   public:
    /** @return Reference to the logger instance */
    static Logger& Get() {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Logs a concatenated message
     * @tparam Args Types of arguments to concatenate
     * @param level Severity level of the message
     * @param args Arguments to concatenate into message
     */
    template <typename... Args>
    void LogConcat(LogLevel level, Args&&... args) {
        std::stringstream ss;
        int dummy[] = {0, ((ss << std::forward<Args>(args)), 0)...};
        (void)dummy;
        Log(level, ss.str());
    }

    /**
     * @brief Logs a formatted message
     * @tparam Args Types of format arguments
     * @param level Severity level of the message
     * @param format Format string
     * @param args Format arguments
     */
    template <typename... Args>
    void LogFormat(LogLevel level, const char* format, Args... args) {
        Log(level, format);  // For now, just output the format string
    }

    void Log(LogLevel level, const std::string& message) {
        std::time_t now = std::time(nullptr);
        char buffer[32];
        std::strftime(buffer, 32, "%c", std::localtime(&now));
        std::string timestamp(buffer);

        const char* colorCode;
        std::string levelStr;
        switch (level) {
            case LogLevel::Trace:
                colorCode = "";
                levelStr = "TRACE";
                break;
            case LogLevel::Info:
                colorCode = "";
                levelStr = "INFO";
                break;
            case LogLevel::Warn:
                colorCode = LogColors::Yellow;
                levelStr = "WARN";
                break;
            case LogLevel::Error:
                colorCode = LogColors::Red;
                levelStr = "ERROR";
                break;
            case LogLevel::Fatal:
                colorCode = LogColors::Purple;
                levelStr = "FATAL";
                break;
        }

        std::cout << "[" << timestamp << "] " << colorCode << "[" << levelStr << "]: " << message
                  << LogColors::Reset << std::endl;
    }

    /**
     * @brief Logs a variable's name and value
     * @tparam T Type of the variable
     * @param level Severity level of the message
     * @param varName Name of the variable
     * @param value Value of the variable
     */
    template <typename T>
    void LogValue(LogLevel level, const char* varName, const T& value) {
        std::stringstream ss;
        ss << varName << " = " << ToString(value);
        Log(level, ss.str());
    }

    /**
     * @brief Logs multiple variables' names and values
     * @tparam Args Types of the variables
     * @param level Severity level of the message
     * @param args Pairs of variable names and values
     */
    template <typename... Args>
    void LogValues(LogLevel level, Args&&... args) {
        std::stringstream ss;
        (LogValueImpl(ss, std::forward<Args>(args)), ...);
        Log(level, ss.str());
    }

   private:
    template <typename T>
    void LogValueImpl(std::stringstream& ss, const char* name, const T& value) {
        ss << name << " = " << ToString(value) << ", ";
    }

    Logger() = default;
};

#define LOG_TRACE_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Trace, __VA_ARGS__)
#define LOG_INFO_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARN_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL_CONCAT(...) Engine::Logger::Get().LogConcat(Engine::LogLevel::Fatal, __VA_ARGS__)

#define LOG_TRACE(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Trace, __VA_ARGS__)
#define LOG_INFO(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) Engine::Logger::Get().LogFormat(Engine::LogLevel::Fatal, __VA_ARGS__)

// New convenience macros
#define LOG_VAR(level, name, value) Engine::Logger::Get().LogValue(level, #name, value)
#define LOG_VARS(level, ...) Engine::Logger::Get().LogValues(level, __VA_ARGS__)
#endif
}  // namespace Engine
