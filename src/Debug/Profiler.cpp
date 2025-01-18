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
ProfilerTimer::ProfilerTimer(std::string_view name)
    : m_Name(name), m_StartTimepoint(std::chrono::steady_clock::now()) 
{
}

// Destructor calculates duration and reports to Profiler
ProfilerTimer::~ProfilerTimer() {
    if (!Profiler::Get().IsEnabled())
        return;

    auto endTimepoint = std::chrono::steady_clock::now();
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
    if (m_AsyncWrites) {
        m_WriteThread = std::thread([this]() { ProcessAsyncWrites(); });
    }
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
    if (m_AsyncWrites) {
        m_StopThread = true;
        m_QueueCV.notify_one();
        if (m_WriteThread.joinable()) {
            m_WriteThread.join();
        }
    }
    
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
        
        std::vector<float> samples;
        for (float time : timings) {
            total += time;
            min = std::min(min, time);
            max = std::max(max, time);
            samples.push_back(time);
        }
        
        float avg = total / static_cast<float>(timings.size());
        
        json profile = {
            {"name", name},
            {"samples", samples},
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

void Profiler::WriteProfile(std::string_view name, float duration) {
    if (!m_Enabled) return;

    // Sample only every Nth call
    uint32_t count = m_CallCounter.fetch_add(1);
    if (count % m_SampleEveryN != 0) return;

    if (m_AsyncWrites) {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_WriteQueue.push({std::string(name), duration});
        m_QueueCV.notify_one();
    } else {
        m_Profiles[std::string(name)].push(duration);
        m_HasUnsavedData = true;
    }
}

void Profiler::ProcessAsyncWrites() {
    while (!m_StopThread) {
        WriteRequest req;
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_QueueCV.wait_for(lock, std::chrono::milliseconds(100), 
                [this] { return !m_WriteQueue.empty() || m_StopThread; });
            
            if (m_WriteQueue.empty()) continue;
            
            req = m_WriteQueue.front();
            m_WriteQueue.pop();
        }
        
        // Process write request
        m_Profiles[std::string(req.name)].push(req.duration);
        m_HasUnsavedData = true;
    }
}

}
