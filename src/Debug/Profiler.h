#pragma once
#include <pch.h>

namespace Engine {

/**
 * @brief RAII-style timer for profiling code blocks
 * 
 * Automatically measures duration between construction and destruction
 * and reports the timing to the Profiler singleton.
 */
class ProfilerTimer {
public:
    /**
     * @brief Starts timing a named block of code
     * @param name Identifier for the profiled code block
     */
    ProfilerTimer(const std::string& name);
    ~ProfilerTimer();
private:
    std::string m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};

/**
 * @brief Singleton class for managing performance profiling
 * 
 * Collects and manages timing data for profiled code blocks.
 * Provides statistical analysis of collected timing data.
 */
class Profiler {
public:
    enum class OutputFormat {
        Console,
        JSON
    };

    /** @return Reference to the singleton Profiler instance */
    static Profiler& Get();
    
    /**
     * @brief Starts a new profiling session
     * @param name Name of the profiling session
     */
    void BeginSession(const std::string& name = "Profile");

    /** @brief Ends current session and outputs results */
    void EndSession();
    
    /**
     * @brief Records a timing measurement
     * @param name Name of the profiled block
     * @param duration Duration in milliseconds
     */
    void WriteProfile(const std::string& name, float duration);
    
    /** @return Whether profiling is currently enabled */
    bool IsEnabled() const { return m_Enabled; }

    /**
     * @brief Enables or disables profiling
     * @param enabled New enabled state
     */
    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    
    /**
     * @brief Sets the output format for profiling results
     * @param format The desired output format
     */
    void SetOutputFormat(OutputFormat format) { m_OutputFormat = format; }
    
    /**
     * @brief Sets the JSON output filepath
     * @param filepath Path where JSON file will be written
     */
    void SetJSONOutputPath(const std::string& filepath) { m_JSONOutputPath = filepath; }

    /** @return Map of all profile names to their timing measurements */
    const std::unordered_map<std::string, std::vector<float>>& GetProfiles() const { return m_Profiles; }

    /** @brief Clears all collected profile data */
    void ClearProfiles() { m_Profiles.clear(); }

    /** @brief Cleanup and save results - called during shutdown */
    void Cleanup();

    /** 
     * @brief Initialize signal handlers - call once at program start
     * @note This is separate from construction to avoid performance impact
     */
    static void InitSignalHandlers();

    /** @brief Set maximum number of samples to keep per profile (0 for unlimited) */
    void SetMaxSamples(size_t count) { m_MaxSamples = count; }
    
    /** @brief Set decimal precision for timing measurements */
    void SetPrecision(int precision) { m_Precision = precision; }

private:
    Profiler();  // Just declare the constructor, no implementation here
    
    void WriteCompleteJSON() const;
    static void SignalHandler(int signal);

    bool m_Enabled;
    std::string m_CurrentSession;
    std::unordered_map<std::string, std::vector<float>> m_Profiles;
    OutputFormat m_OutputFormat;
    std::string m_JSONOutputPath;
    bool m_HasUnsavedData = false;
    static bool s_SignalsInitialized;
    size_t m_MaxSamples = 1000; // Keep last 1000 samples per profile
    int m_Precision = 3;    // 3 decimal places for ms
};

}

/**
 * @brief Creates a profiler timer for the current scope
 * @param name Name of the profiled block
 */
#define PROFILE_SCOPE(name) Engine::ProfilerTimer timer##__LINE__(name)

/**
 * @brief Profiles the current function
 * 
 * Creates a profiler timer using the current function name
 */
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
