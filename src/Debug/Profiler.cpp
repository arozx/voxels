/**
 * @file Profiler.cpp
 * @brief Implementation of the performance profiling system
 * 
 * Provides implementation for the ProfilerTimer and Profiler classes,
 * handling timing measurements and statistical analysis of profiled code blocks.
 */
#include <pch.h>
#include "Profiler.h"
#include <csignal>
#include <iomanip>

namespace Engine {

// Constructor starts timing measurement
ProfilerTimer::ProfilerTimer(const std::string& name)
    : m_Name(name), m_StartTimepoint(std::chrono::high_resolution_clock::now()) 
{
}

// Destructor calculates duration and reports to Profiler
ProfilerTimer::~ProfilerTimer() {
    if (!Profiler::Get().IsEnabled())
        return;

    auto endTimepoint = std::chrono::high_resolution_clock::now();
    auto start = m_StartTimepoint;
    auto duration = std::chrono::duration<float, std::milli>(endTimepoint - start).count();
    
    Profiler::Get().WriteProfile(m_Name, duration);
}

// Returns singleton instance
Profiler& Profiler::Get() {
    static Profiler instance;
    return instance;
}

bool Profiler::s_SignalsInitialized = false;

Profiler::Profiler() 
    : m_Enabled(true)
    , m_OutputFormat(OutputFormat::Console)
    , m_JSONOutputPath("profile_results.json")
    , m_HasUnsavedData(false)
{
}

void Profiler::InitSignalHandlers() {
    if (!s_SignalsInitialized) {
        std::signal(SIGINT, SignalHandler);
        std::signal(SIGTERM, SignalHandler);
        s_SignalsInitialized = true;
    }
}

void Profiler::SignalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", saving profiler results...\n";
    Get().Cleanup();
    std::exit(signal);
}

void Profiler::Cleanup() {
    if (m_HasUnsavedData) {
        WriteCompleteJSON();
        m_HasUnsavedData = false;
    }
}

// Begins a new profiling session
void Profiler::BeginSession(const std::string& name) {
    m_CurrentSession = name;
    m_Profiles.clear();
}

// Ends session and outputs statistical results
void Profiler::EndSession() {
    if (!m_Enabled) return;

    switch (m_OutputFormat) {
        case OutputFormat::JSON:
            WriteCompleteJSON();
            break;
        case OutputFormat::Console:
            std::cout << "Profile results for session '" << m_CurrentSession << "':\n";
            for (const auto& [name, timings] : m_Profiles) {
                if (timings.empty()) continue;
                
                float total = 0.0f;
                for (float time : timings)
                    total += time;
                
                float avg = total / static_cast<float>(timings.size());
                float min = *std::min_element(timings.begin(), timings.end());
                float max = *std::max_element(timings.begin(), timings.end());
                
                std::cout << name << ": Avg: " << avg << "ms, Min: " << min 
                    << "ms, Max: " << max << "ms, Calls: " << timings.size() << "\n";
            }
            break;
    }
}

void Profiler::WriteCompleteJSON() const {
    using json = nlohmann::json;
    json output;
    
    output = {
        {"session", m_CurrentSession},
        {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
        {"profiles", json::array()}
    };
    
    for (const auto& [name, timings] : m_Profiles) {
        if (timings.empty()) continue;
        
        float total = 0.0f;
        float min = timings[0];
        float max = timings[0];
        
        for (float time : timings) {
            total += time;
            min = std::min(min, time);
            max = std::max(max, time);
        }
        
        float avg = total / static_cast<float>(timings.size());
        
        json profile = {
            {"name", name},
            {"samples", timings},
            {"calls", timings.size()},
            {"averageMs", avg},
            {"minMs", min},
            {"maxMs", max}
        };
        
        output["profiles"].push_back(profile);
    }
    
    try {
        std::ofstream file(m_JSONOutputPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << m_JSONOutputPath << std::endl;
            return;
        }
        file << std::setw(2) << output;
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error writing profile data: " << e.what() << std::endl;
    }
}

void Profiler::WriteProfile(const std::string& name, float duration) {
    if (!m_Enabled) return;
    
    auto& samples = m_Profiles[name];
    samples.push_back(duration);
    
    if (m_MaxSamples > 0 && samples.size() > m_MaxSamples) {
        samples.erase(samples.begin(), samples.begin() + (samples.size() - m_MaxSamples));
    }
    
    m_HasUnsavedData = true;

    // Write immediately if in JSON mode
    if (m_OutputFormat == OutputFormat::JSON) {
        WriteCompleteJSON();
        m_HasUnsavedData = false;
    }
}

}
