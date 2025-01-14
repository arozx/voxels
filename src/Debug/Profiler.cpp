/**
 * @file Profiler.cpp
 * @brief Implementation of the performance profiling system
 * 
 * Provides implementation for the ProfilerTimer and Profiler classes,
 * handling timing measurements and statistical analysis of profiled code blocks.
 */
#include "pch.h"
#include "Profiler.h"

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

// Begins a new profiling session
void Profiler::BeginSession(const std::string& name) {
    m_CurrentSession = name;
    m_Profiles.clear();
}

// Ends session and outputs statistical results
void Profiler::EndSession() {
    if (!m_Enabled) return;

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
}

// Records a timing measurement
void Profiler::WriteProfile(const std::string& name, float duration) {
    if (!m_Enabled) return;
    m_Profiles[name].push_back(duration);
}

}
