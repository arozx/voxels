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
thread_local static std::chrono::steady_clock::time_point s_LastSampleTime = std::chrono::steady_clock::now();
static constexpr std::chrono::duration<float> SAMPLE_INTERVAL{1.0f/60.0f}; // 60Hz sampling

/**
     * @brief Constructs a timer for performance profiling.
     *
     * Initializes a profiler timer with the given name and records the current time point
     * as the start of the measurement. When the timer goes out of scope, it will automatically
     * log the elapsed time using the Profiler singleton.
     *
     * @param name A string view representing the name of the profiled code segment.
     * @note The timer uses std::chrono::steady_clock for precise time measurement.
     */
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

/**
     * @brief Default constructor for the Profiler class.
     *
     * Initializes a Profiler instance with default settings:
     * - Profiling is disabled by default
     * - Output format is set to console
     * - Default JSON output path is set to "profile_results.json"
     * - No unsaved profiling data initially
     *
     * @note This constructor sets up the initial state of the profiler without starting a profiling session.
     * Use BeginSession() to activate profiling and start collecting performance data.
     */
    Profiler::Profiler() 
    : m_Enabled(false)
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
    LOG_INFO("Received signal ", signal, ", saving profiler results...");
    Get().Cleanup();
    std::exit(signal);
}

void Profiler::Cleanup() {
    if (m_HasUnsavedData) {
        WriteCompleteJSON();
        m_HasUnsavedData = false;
    }
}

/**
 * @brief Begins a new profiling session and initializes the JSON output file.
 *
 * @details This method starts a new profiling session by:
 * - Setting the current session name
 * - Marking unsaved data flag
 * - Creating a JSON structure with session metadata
 * - Attempting to write the initial JSON structure to the output file
 *
 * @param name The name of the profiling session to be started
 *
 * @note Thread-safe method using a mutex lock
 * @note Logs an error if file initialization fails
 * @note Logs an informational message when the session starts
 *
 * @throws std::exception If file writing encounters an error
 */
void Profiler::BeginSession(const std::string& name) {
    std::unique_lock lock(m_Mutex);
    m_CurrentSession = name;
    m_HasUnsavedData = true;
    
    // Initialize empty JSON structure
    using nlohmann::json;
    json output;
    output["session"] = name;
    output["profiles"] = json::array();
    output["timestamp"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    try {
        std::ofstream file(m_JSONOutputPath);
        if (file) {
            file << std::setw(2) << output;
            file.flush();
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize profile file: ", e.what());
    }

    LOG_INFO("Started profiling session: ", name);
}

/**
 * @brief Ends the current profiling session and outputs statistical results.
 *
 * Terminates the active profiling session and generates output based on the configured output format.
 * If profiling is not enabled or frame profiling is active, the method returns without action.
 *
 * @details Supports two output formats:
 * - JSON: Writes complete profiling data to a JSON file using WriteCompleteJSON()
 * - Console: Logs profile statistics for each tracked profile, including average, minimum, 
 *   maximum times, and total number of calls
 *
 * @note Skips profiles with no recorded data
 * @note Requires profiling to be enabled via BeginSession()
 *
 * @pre Profiling session must be active
 * @post Profiling session is terminated and results are output
 */
void Profiler::EndSession() {
    if (!m_Enabled) return;
    if (m_ProfilingFrames) return;

    switch (m_OutputFormat) {
        case OutputFormat::JSON:
            WriteCompleteJSON();
            break;
        case OutputFormat::Console:
            LOG_INFO("Profile results for session", m_CurrentSession);
            for (const auto& [name, data] : m_Profiles) {
                if (data->empty()) continue;

                LOG_INFO(std::string_view(name), ": Avg: ", (data->totalTime / data->calls),
                         "ms, Min: ", data->minTime, "ms, Max: ", data->maxTime,
                         "ms, Calls: ", data->calls);
            }
            break;
    }
}

/**
 * @brief Writes complete profiling data to a JSON file.
 *
 * This method serializes profiling information collected during a session into a JSON format.
 * It includes session metadata, individual profile statistics, and optional frame performance data.
 *
 * @details The method performs the following key operations:
 * - Checks if profiling is enabled
 * - Creates a JSON structure with session details and timestamp
 * - Aggregates profile data including call counts, timing statistics, and individual samples
 * - Optionally includes frame performance data
 * - Writes the complete JSON output to a file specified by m_JSONOutputPath
 *
 * @note Thread-safe method using shared locking mechanism
 * @note Skips profiles with zero calls or invalid data
 * @note Handles both local and dynamic sample storage
 *
 * @throws std::exception If file writing encounters an error
 * @pre Profiling must be enabled
 * @post JSON file is created/overwritten with complete profiling data
 */
void Profiler::WriteCompleteJSON() const {
    if (!m_Enabled) return;

    using nlohmann::json;
    json output;
    
    std::shared_lock lock(m_Mutex);
    
    output["session"] = m_CurrentSession;
    output["timestamp"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    output["profiles"] = json::array();
    
    // Add profile data first
    for (const auto& [name, data] : m_Profiles) {
        if (!data || data->calls == 0) continue;
        
        json profile = {
            {"name", std::string(std::string_view(name))},
            {"calls", data->calls},
            {"averageMs", data->avgTime},
            {"recentMs", data->recentAvg},
            {"minMs", data->minTime},
            {"maxMs", data->maxTime},
            {"totalMs", data->totalTime}
        };

        // Add samples array
        profile["samples"] = json::array();
        if (data->localCount <= data->LOCAL_SAMPLES) {
            for (uint32_t i = 0; i < data->localCount; ++i) {
                profile["samples"].push_back(data->localSamples[i]);
            }
        } else {
            for (const auto& sample : data->samples) {
                profile["samples"].push_back(sample);
            }
        }
        
        output["profiles"].push_back(std::move(profile));
    }
    
    // Add frame data separately
    if (m_ProfilingFrames || !m_FrameData.empty()) {
        output["frame_data"] = json::array();
        for (const auto& frame : m_FrameData) {
            output["frame_data"].push_back({
                {"frame_number", frame.frameNumber},
                {"frame_time_ms", frame.frameTime},
                {"timestamp", std::chrono::system_clock::to_time_t(frame.timestamp)}
            });
        }
    }
    
    lock.unlock();
    
    try {
        std::ofstream file(m_JSONOutputPath, std::ios::out | std::ios::trunc);
        if (!file) {
            LOG_ERROR("Failed to open profile output file: ", m_JSONOutputPath);
            return;
        }
        file << std::setw(2) << output << std::endl;
        file.close();
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error writing profile data: ", e.what());
    }
}

/**
 * @brief Writes frame profiling data to a JSON file.
 *
 * Generates a comprehensive JSON output containing frame-level performance metrics
 * and statistical analysis of frame times during profiling. The output includes:
 * - Session information
 * - Total number of frames profiled
 * - Timestamp of data generation
 * - Detailed frame-by-frame timing data
 * - Frame performance statistics
 *
 * @details The method performs the following key operations:
 * - Captures frame number, frame time, and timestamp for each recorded frame
 * - Calculates frame time statistics including average, minimum, and maximum
 * - Computes frame rate (FPS) metrics based on frame time calculations
 * - Writes the generated JSON data to a file specified by m_FramesJSONPath
 *
 * @note Uses nlohmann::json for JSON serialization
 * @note Handles potential file writing exceptions with error logging
 * @note Skips statistics generation if no frame data is available
 *
 * @throws std::exception If file writing encounters an error
 */
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
        LOG_ERROR("Error waiting for profile data", e.what());
    }
}

/**
 * @brief Records a performance profile sample with rate limiting and thread-safe data management.
 *
 * Adds a performance measurement sample to the profiler, ensuring thread safety, 
 * preventing excessive sampling, and periodically writing profiling data to JSON.
 *
 * @param name A string view representing the profile/function name being measured
 * @param duration The duration of the profile sample in floating-point seconds
 *
 * @note Samples are rate-limited to 60Hz to prevent performance overhead
 * @note Skips recording if profiling is disabled or profile name is empty
 * @note Writes complete JSON profile data every 500 milliseconds during profiling
 *
 * @thread_safety Thread-safe with shared and unique locks for concurrent access
 */
void Profiler::WriteProfile(std::string_view name, float duration) {
    if (!m_Enabled || name.empty()) return;
    
    // Rate limit samples to 60Hz
    auto currentTime = std::chrono::steady_clock::now();
    if (currentTime - s_LastSampleTime < SAMPLE_INTERVAL) {
        return;
    }
    s_LastSampleTime = currentTime;
    
    ProfileName profName(name);
    ProfileData* data = nullptr;
    
    {
        std::shared_lock readLock(m_Mutex);
        data = t_FastPath.find(profName);
    }
    
    if (!data) {
        std::unique_lock lock(m_Mutex);
        auto it = m_Profiles.find(profName);
        if (it == m_Profiles.end()) {
            data = m_DataPool.allocate();
            it = m_Profiles.emplace(std::move(profName), data).first;
            t_FastPath.insert(it->first, data);
        } else {
            data = it->second;
            t_FastPath.insert(it->first, data);
        }
    }
    
    data->AddSample(duration);
    m_HasUnsavedData = true;
    
    // Write more frequently for development
    if ((currentTime - m_LastWriteTime) >= std::chrono::milliseconds(500)) {  // Write every 500ms
        WriteCompleteJSON();
        m_LastWriteTime = currentTime;
    }
}

/**
 * @brief Processes and flushes batched performance measurements.
 *
 * This method handles the processing of accumulated performance measurements. It performs the following tasks:
 * - Checks if there are any batched measurements to process
 * - Acquires a lock to ensure thread-safe access to shared data structures
 * - Adds each batched measurement to its corresponding profile
 * - Clears the batch of measurements
 * - Marks the presence of unsaved profiling data
 * - If the output format is JSON, writes the complete profiling data and clears the unsaved data flag
 *
 * @note Thread-safe method using a unique lock to protect shared resources
 * @note No-op if no batched measurements exist
 * @note Automatically triggers JSON output if JSON format is selected
 */
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
    LOG_INFO("Profiling next ", frameCount, " frames.");
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
