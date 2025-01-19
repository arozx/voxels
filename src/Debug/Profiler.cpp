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
#include <charconv>

namespace Engine {

// Constructor starts timing measurement
ProfilerTimer::ProfilerTimer(std::string_view name)
    : m_Name(name), m_StartTimepoint(std::chrono::steady_clock::now()) {}

ProfilerTimer::~ProfilerTimer() {
    if (!Profiler::Get().IsEnabled())
        return;

    auto endTimepoint = std::chrono::steady_clock::now();
    auto duration = Profiler::Get().IsHighPrecision() ?
        std::chrono::duration<float, std::micro>(endTimepoint - m_StartTimepoint).count() / 1000.0f :
        std::chrono::duration<float, std::milli>(endTimepoint - m_StartTimepoint).count();
    
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
    , m_HasUnsavedData(false) {}

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
    m_StringPool.reserve(INITIAL_POOL_CAPACITY);
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
            for (const auto& [name, data] : m_Profiles) {
                if (data->empty()) continue;
                
                std::cout << std::string_view(name) << ": Avg: " 
                    << (data->totalTime / data->calls) << "ms, Min: "
                    << data->minTime << "ms, Max: " << data->maxTime
                    << "ms, Calls: " << data->calls << "\n";
            }
            break;
    }
}

void Profiler::WriteCompleteJSON() const {
    using nlohmann::json;  // Add using declaration here
    static thread_local json output; // Reuse JSON object
    output.clear();
    
    output["session"] = m_CurrentSession;
    output["timestamp"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    output["profiles"] = json::array();  // Initialize array without reserve
    
    auto& profiles = output["profiles"];
    for (const auto& [name, data] : m_Profiles) {
        if (data->calls == 0) continue;
        
        profiles.push_back({
            {"name", std::string_view(name)},
            {"calls", data->calls},
            {"averageMs", data->avgTime},
            {"recentMs", data->recentAvg}, // Add recent average
            {"minMs", data->minTime},
            {"maxMs", data->maxTime}
        });
    }
    
    try {
        std::ofstream file(m_JSONOutputPath);
        if (file) {
            file << std::setw(2) << output;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error writing profile data: " << e.what() << std::endl;
    }
}

void Profiler::WriteProfile(std::string_view name, float duration) {
    if (!m_Enabled) return;
    
    if (m_BatchSize > 0) {
        ReserveBatch();
        m_BatchedMeasurements.emplace_back(name, duration);
        if (m_BatchedMeasurements.size() >= m_BatchSize) {
            FlushBatch();
        }
        return;
    }
    
    // Try fast path first
    ProfileName profName(name);
    if (auto* data = t_FastPath.find(profName)) {
        data->AddSample(duration);
    } else {
        // Slow path with lock
        std::unique_lock lock(m_Mutex);
        auto it = m_Profiles.find(profName);
        if (it == m_Profiles.end()) {
            auto* data = m_DataPool.allocate();
            it = m_Profiles.emplace(std::move(profName), data).first;
            t_FastPath.insert(it->first, data);
        } else {
            t_FastPath.insert(it->first, it->second);
        }
        it->second->AddSample(duration);
    }
    
    m_HasUnsavedData = true;
    
    auto now = std::chrono::steady_clock::now();
    if (m_OutputFormat == OutputFormat::JSON && 
        (now - m_LastWriteTime) >= WRITE_INTERVAL) {
        WriteCompleteJSON();
        m_HasUnsavedData = false;
        m_LastWriteTime = now;
    }
}

void Profiler::FlushBatch() {
    if (m_BatchedMeasurements.empty()) return;
    
    std::unique_lock lock(m_Mutex);
    for (const auto& entry : m_BatchedMeasurements) {
        auto& profile = m_Profiles[entry.name];  // Use ProfileName directly
        profile->AddSample(entry.duration);
    }
    m_BatchedMeasurements.clear();
    m_HasUnsavedData = true;
    
    if (m_OutputFormat == OutputFormat::JSON) {
        WriteCompleteJSON();
        m_HasUnsavedData = false;
    }
}

std::string_view Profiler::InternString(std::string_view str) {
    {
        std::shared_lock read_lock(m_Mutex);
        auto it = m_StringPool.find(std::string(str));
        if (it != m_StringPool.end()) {
            return *it;
        }
    }
    
    std::unique_lock write_lock(m_Mutex);
    return *m_StringPool.insert(std::string(str)).first;
}

// Initialize thread local storage
thread_local Profiler::ThreadLocalBuffer Profiler::t_StringBuffer;
thread_local Profiler::FastPathCache Profiler::t_FastPath;

thread_local std::vector<Profiler::BatchEntry> Profiler::m_BatchedMeasurements;

}
