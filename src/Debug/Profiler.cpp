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

thread_local std::vector<Profiler::BatchEntry> Profiler::m_BatchedMeasurements;
thread_local Profiler::ThreadLocalBuffer Profiler::t_StringBuffer;
thread_local Profiler::FastPathCache Profiler::t_FastPath;

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
    LOG_INFO_CONCAT("Received signal ", signal, ", saving profiler results...");
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
    if (m_ProfilingFrames) return;

    switch (m_OutputFormat) {
        case OutputFormat::JSON:
            WriteCompleteJSON();
            break;
        case OutputFormat::Console:
            LOG_INFO_CONCAT("Profile results for session", m_CurrentSession);
            for (const auto& [name, data] : m_Profiles) {
                if (data->empty()) continue;
                
                LOG_INFO_CONCAT(std::string_view(name), ": Avg: ", 
                (data->totalTime / data->calls), "ms, Min: ",
                data->minTime, "ms, Max: ", data->maxTime,
                "ms, Calls: ", data->calls);
            }
            break;
    }
}

void Profiler::WriteCompleteJSON() const {
    using nlohmann::json;
    static thread_local json output;
    output.clear();
    
    output["session"] = m_CurrentSession;
    output["timestamp"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Add frame profiling data if available
    if (m_ProfilingFrames || !m_FrameData.empty()) {
        output["frame_data"] = json::array();
        for (const auto& frame : m_FrameData) {
            output["frame_data"].push_back({
                {"frame_number", frame.frameNumber},
                {"frame_time_ms", frame.frameTime},
                {"timestamp", std::chrono::system_clock::to_time_t(frame.timestamp)}
            });
        }
        
        // Add frame statistics
        if (!m_FrameData.empty()) {
            float totalTime = 0.0f;
            float minTime = std::numeric_limits<float>::max();
            float maxTime = 0.0f;
            
            for (const auto& frame : m_FrameData) {
                totalTime += frame.frameTime;
                minTime = std::min(minTime, frame.frameTime);
                maxTime = std::max(maxTime, frame.frameTime);
            }
            
            output["frame_stats"] = {
                {"total_frames", m_FrameData.size()},
                {"average_frame_time_ms", totalTime / m_FrameData.size()},
                {"min_frame_time_ms", minTime},
                {"max_frame_time_ms", maxTime}
            };
        }
    }
    
    // Add regular profile data
    output["profiles"] = json::array();
    for (const auto& [name, data] : m_Profiles) {
        if (data->calls == 0) continue;
        
        output["profiles"].push_back({
            {"name", std::string_view(name)},
            {"calls", data->calls},
            {"averageMs", data->avgTime},
            {"recentMs", data->recentAvg},
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
        LOG_ERROR_CONCAT("Error writing profile data", e.what());
    }
}

void Profiler::WriteFrameDataJSON() const {
    using nlohmann::json;
    json output;
    
    output["session"] = m_CurrentSession;
    output["frames_profiled"] = m_FramesToProfile;
    output["timestamp"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Add detailed frame data
    output["frames"] = json::array();
    for (const auto& frame : m_FrameData) {
        output["frames"].push_back({
            {"frame_number", frame.frameNumber},
            {"frame_time_ms", frame.frameTime},
            {"timestamp", std::chrono::system_clock::to_time_t(frame.timestamp)}
        });
    }
    
    // Add frame statistics
    if (!m_FrameData.empty()) {
        float totalTime = 0.0f;
        float minTime = std::numeric_limits<float>::max();
        float maxTime = 0.0f;
        
        for (const auto& frame : m_FrameData) {
            totalTime += frame.frameTime;
            minTime = std::min(minTime, frame.frameTime);
            maxTime = std::max(maxTime, frame.frameTime);
        }
        
        output["statistics"] = {
            {"total_frames", m_FrameData.size()},
            {"average_frame_time_ms", totalTime / m_FrameData.size()},
            {"min_frame_time_ms", minTime},
            {"max_frame_time_ms", maxTime},
            {"fps_average", 1000.0f / (totalTime / m_FrameData.size())},
            {"fps_highest", 1000.0f / minTime},
            {"fps_lowest", 1000.0f / maxTime}
        };
    }
    
    try {
        std::ofstream file(m_FramesJSONPath);
        if (file) {
            file << std::setw(2) << output;
        }
    } catch (const std::exception& e) {
        LOG_ERROR_CONCAT("Error waiting for profile data", e.what());
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
    
    // disable auto-write during frame profiling
    if (!m_ProfilingFrames) {
        auto now = std::chrono::steady_clock::now();
        if (m_OutputFormat == OutputFormat::JSON && 
            (now - m_LastWriteTime) >= WRITE_INTERVAL) {
            WriteCompleteJSON();
            m_HasUnsavedData = false;
            m_LastWriteTime = now;
        }
    }
}

void Profiler::FlushBatch() {
    if (m_BatchedMeasurements.empty()) return;
    
    std::unique_lock lock(m_Mutex);
    for (const auto& entry : m_BatchedMeasurements) {
        auto& profile = m_Profiles[entry.name];
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

bool Profiler::EndFrame() {
    if (!m_ProfilingFrames) return false;
    
    // Calculate frame time
    static auto lastFrameTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    float frameTime = std::chrono::duration<float, std::milli>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
    
    // Record frame data
    m_FrameData.emplace_back(m_CurrentProfiledFrame, frameTime);
    
    m_CurrentProfiledFrame++;
    
    if (m_CurrentProfiledFrame >= m_FramesToProfile) {
        m_ProfilingFrames = false;
        WriteFrameDataJSON();  // Write frame data to separate file
        m_FrameData.clear();
        
        RestoreProfilingState();  // Restore previous profiling state
        LOG_INFO("Frame profiling complete, resumed normal profiling.");
        return false;
    }
    return true;
}

void Profiler::PreserveProfilingState() {
    // Deep copy the existing profiles
    m_RegularProfiles.clear();  // Ensure it's empty first
    for (const auto& [name, data] : m_Profiles) {
        auto* newData = m_DataPool.allocate();
        *newData = *data;  // Copy the data
        m_RegularProfiles[name] = newData;
    }
    m_RegularSession = m_CurrentSession;  // Save the current session name (e.g. "Runtime")
    m_Profiles.clear();
}

void Profiler::RestoreProfilingState() {
    // Clean up current frame profiling data
    for (auto& [name, data] : m_Profiles) {
        m_DataPool.deallocate(data);
    }
    m_Profiles.clear();
    t_FastPath = FastPathCache();  // Reset fast path cache
    
    // Restore the original profiles
    for (const auto& [name, data] : m_RegularProfiles) {
        auto* newData = m_DataPool.allocate();
        *newData = *data;  // Copy the data back
        m_Profiles[name] = newData;
        t_FastPath.insert(name, newData);  // Re-populate fast path cache
    }
    
    // Clean up temporary storage
    for (auto& [name, data] : m_RegularProfiles) {
        m_DataPool.deallocate(data);
    }
    m_RegularProfiles.clear();
    
    // Restore original session name and force a write
    m_CurrentSession = m_RegularSession;  // Directly restore session name
    m_HasUnsavedData = true;  // Force a write of restored data
    WriteCompleteJSON();  // Write the restored data immediately
    
    // Reset frame-specific state
    m_ProfilingFrames = false;
    m_CurrentProfiledFrame = 0;
    m_FrameData.clear();
    m_BatchedMeasurements.clear();
    
    // Restore fast path cache and enable profiling
    m_Enabled = true;
    t_FastPath = FastPathCache(); // Reset cache
    
    // Restore session and force a write
    BeginSession(m_RegularSession); // Use BeginSession instead of direct assignment
    
    // Re-register all existing profiles in the fast path cache
    for (const auto& [name, data] : m_Profiles) {
        t_FastPath.insert(name, data);
    }

    m_HasUnsavedData = true;
    m_LastWriteTime = std::chrono::steady_clock::now();
    WriteCompleteJSON();
}

void Profiler::ProfileFrames(uint32_t frameCount) {
    frameCount = std::min(frameCount, MAX_PROFILE_FRAMES);
    LOG_INFO_CONCAT("Profiling next ", frameCount, " frames.");
    PreserveProfilingState();  // Save current state
    m_CurrentSession = "Frame Profile";
    m_FramesToProfile = frameCount;
    m_CurrentProfiledFrame = 0;
    m_ProfilingFrames = true;
    
    // Reserve space for frame data
    m_FrameData.clear();
    m_FrameData.reserve(frameCount);
}
}
