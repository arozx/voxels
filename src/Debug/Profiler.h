#pragma once
#include <pch.h>
#include <unordered_set>
#include <shared_mutex>
#include <cstring>

namespace Engine {

/**
 * @brief RAII-style timer for profiling code blocks
 * 
 * Automatically measures duration between construction and destruction
 * and reports the timing to the Profiler singleton.
 */
class ProfilerTimer {
public:
    explicit ProfilerTimer(std::string_view name);
    ~ProfilerTimer();
private:
    const std::string_view m_Name;  // Changed from std::string to std::string_view
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
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

    static constexpr uint32_t MAX_PROFILE_FRAMES = 1000;

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
    void WriteProfile(std::string_view name, float duration);
    
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
    std::unordered_map<std::string, std::vector<float>> GetProfiles() const {
        std::unordered_map<std::string, std::vector<float>> result;
        for (const auto& [name, data] : m_Profiles) {
            result[std::string(std::string_view(name))] = data->samples;
        }
        return result;
    }

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

    /** @brief Set batch size for JSON writes (0 for immediate write) */
    void SetBatchSize(size_t size) { m_BatchSize = size; }
    
    /** @brief Set high precision timing mode */
    void SetHighPrecision(bool enabled) { m_HighPrecision = enabled; }

    /** @return Whether high precision timing is enabled */
    bool IsHighPrecision() const { return m_HighPrecision; }

    /**
     * @brief Starts profiling for a specific number of frames
     * @param frameCount Number of frames to profile (capped at MAX_PROFILE_FRAMES)
     */
    void ProfileFrames(uint32_t frameCount);

    /**
     * @brief Marks the end of a frame, used for frame profiling
     * @return true if still profiling frames, false if done
     */
    bool EndFrame();  // Remove implementation, keep only declaration

    /** @return Whether currently profiling frames */
    bool IsProfilingFrames() const { return m_ProfilingFrames; }

    /** @return Current frame being profiled */
    uint32_t GetCurrentProfiledFrame() const { return m_CurrentProfiledFrame; }

    /** @brief Check if profiler is properly initialized */
    bool IsInitialized() const { 
        return m_Enabled && !m_CurrentSession.empty(); 
    }

private:
    Profiler();
    
    void WriteCompleteJSON() const;
    static void SignalHandler(int signal);

    static constexpr size_t INITIAL_VECTOR_CAPACITY = 1000;
    static constexpr size_t INITIAL_POOL_CAPACITY = 1000;
    static constexpr size_t BATCH_RESERVE_SIZE = 128;
    static constexpr size_t SMALL_STRING_SIZE = 64;  // For small string optimization
    static constexpr size_t SMALL_VECTOR_SIZE = 16;
    static constexpr size_t MEMORY_POOL_SIZE = 256;
    static constexpr size_t STRING_BUFFER_SIZE = 1024;
    static constexpr size_t FAST_PATH_SIZE = 32;

    struct ProfileData;

    // Small string optimization for profile names
    struct ProfileName {
        char data[SMALL_STRING_SIZE];
        size_t length;
        
        // Constructor
        ProfileName(std::string_view sv) {
            length = std::min(sv.length(), SMALL_STRING_SIZE - 1);
            memcpy(data, sv.data(), length);
            data[length] = '\0';
        }

        ProfileName() : length(0) {
            data[0] = '\0';
        }

        // Special member functions
        ProfileName(const ProfileName& other) noexcept 
            : length(other.length) {
            memcpy(data, other.data, length + 1);
        }

        ProfileName& operator=(const ProfileName& other) noexcept {
            if (this != &other) {
                length = other.length;
                memcpy(data, other.data, length + 1);
            }
            return *this;
        }

        ProfileName(ProfileName&& other) noexcept 
            : length(other.length) {
            memcpy(data, other.data, length + 1);
        }

        ProfileName& operator=(ProfileName&& other) noexcept {
            if (this != &other) {
                length = other.length;
                memcpy(data, other.data, length + 1);
            }
            return *this;
        }

        operator std::string_view() const { return std::string_view(data, length); }
        
        bool operator==(const ProfileName& other) const {
            return length == other.length && 
                memcmp(data, other.data, length) == 0;
        }
        
        size_t hash() const noexcept {
            size_t h = 14695981039346656037ULL;
            for (size_t i = 0; i < length; ++i) {
                h ^= static_cast<size_t>(data[i]);
                h *= 1099511628211ULL;
            }
            return h;
        }
    };

    struct ProfileNameHash {
        size_t operator()(const ProfileName& name) const noexcept {
            return name.hash();
        }
    };

    template<typename T, size_t N>
    class SmallVector {
        alignas(T) unsigned char inlineData[N * sizeof(T)];
        std::vector<T> heap;
        T* data{reinterpret_cast<T*>(inlineData)};
        size_t size_{0};
        bool isInline{true};

    public:
        void push_back(const T& value) {
            if (isInline && size_ < N) {
                new(data + size_) T(value);
            } else {
                if (isInline) {
                    heap.reserve(INITIAL_VECTOR_CAPACITY);
                    heap.insert(heap.end(), data, data + size_);
                    isInline = false;
                }
                heap.push_back(value);
            }
            size_++;
        }

        void clear() {
            if (isInline) {
                for (size_t i = 0; i < size_; i++) {
                    data[i].~T();
                }
            } else {
                heap.clear();
            }
            size_ = 0;
        }

        T* begin() { return isInline ? data : heap.data(); }
        T* end() { return isInline ? data + size_ : heap.data() + size_; }
        const T* begin() const { return isInline ? data : heap.data(); }
        const T* end() const { return isInline ? data + size_ : heap.data() + size_; }
        bool empty() const { return size_ == 0; }
        size_t size() const { return size_; }

        operator std::vector<T>() const {
            if (isInline) {
                return std::vector<T>(data, data + size_);
            }
            return heap;
        }

        // Add reserve method
        void reserve(size_t newCap) {
            if (!isInline) {
                heap.reserve(newCap);
            }
            // No need to reserve for inline data
        }

        // Move samples to heap without reserve
        void moveToHeap() {
            if (isInline) {
                heap.insert(heap.end(), data, data + size_);
                isInline = false;
            }
        }

        T& back() { return isInline ? data[size_ - 1] : heap.back(); }
        const T& back() const { return isInline ? data[size_ - 1] : heap.back(); }
        
        // Add method to set last element
        void set_back(const T& value) {
            if (isInline) {
                data[size_ - 1] = value;
            } else {
                heap.back() = value;
            }
        }

        // Add operator[] overloads
        T& operator[](size_t index) {
            if (isInline) {
                return data[index];
            }
            return heap[index];
        }
        
        const T& operator[](size_t index) const {
            if (isInline) {
                return data[index];
            }
            return heap[index];
        }
    };

    struct alignas(64) ProfileData {
        static constexpr size_t LOCAL_SAMPLES = 8;
        static constexpr size_t MAX_SAMPLES = 1000; // Limit total samples
        
        float localSamples[LOCAL_SAMPLES];
        SmallVector<float, SMALL_VECTOR_SIZE> samples;
        uint32_t calls{0};
        uint32_t localCount{0};
        float minTime{std::numeric_limits<float>::max()};
        float maxTime{std::numeric_limits<float>::lowest()};
        float totalTime{0.0f};
        float avgTime{0.0f};
        float recentAvg{0.0f}; // Moving average for recent samples

        void AddSample(float time) noexcept {
            calls++;
            
            // Update moving average
            constexpr float alpha = 0.1f; // Smoothing factor
            recentAvg = alpha * time + (1.0f - alpha) * recentAvg;
            
            if (localCount < LOCAL_SAMPLES) {
                localSamples[localCount++] = time;
            } else {
                if (localCount == LOCAL_SAMPLES) {
                    samples.moveToHeap();
                    for (uint32_t i = 0; i < LOCAL_SAMPLES; i++) {
                        samples.push_back(localSamples[i]);
                    }
                    localCount++;
                }
                
                // Implement circular buffer behavior when max samples reached
                if (samples.size() >= MAX_SAMPLES) {
                    // Shift samples left, discarding oldest
                    std::rotate(samples.begin(), samples.begin() + 1, samples.end());
                    samples.set_back(time);
                } else {
                    samples.push_back(time);
                }
            }
            
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
            totalTime += time;
            avgTime = totalTime / static_cast<float>(calls);
        }

        bool empty() const { return samples.empty(); }
        size_t size() const { return samples.size(); }
        auto begin() const { return samples.begin(); }
        auto end() const { return samples.end(); }

        // Add copy constructor and assignment operator
        ProfileData& operator=(const ProfileData& other) {
            if (this != &other) {
                samples = other.samples;
                calls = other.calls;
                localCount = other.localCount;
                minTime = other.minTime;
                maxTime = other.maxTime;
                totalTime = other.totalTime;
                avgTime = other.avgTime;
                recentAvg = other.recentAvg;
                std::memcpy(localSamples, other.localSamples, sizeof(localSamples));
            }
            return *this;
        }
    };

    class ProfileDataPool {
        std::array<ProfileData, MEMORY_POOL_SIZE> pool;
        std::array<bool, MEMORY_POOL_SIZE> used{};
        std::mutex poolMutex;

    public:
        ProfileData* allocate() {
            std::lock_guard lock(poolMutex);
            for (size_t i = 0; i < MEMORY_POOL_SIZE; i++) {
                if (!used[i]) {
                    used[i] = true;
                    return &pool[i];
                }
            }
            return new ProfileData(); // Fallback to heap
        }

        void deallocate(ProfileData* data) {
            std::lock_guard lock(poolMutex);
            auto idx = data - pool.data();
            if (idx >= 0 && idx < MEMORY_POOL_SIZE) {
                used[idx] = false;
                data->~ProfileData();
            } else {
                delete data;
            }
        }
    };

    ProfileDataPool m_DataPool;
    
    using ProfileMap = std::unordered_map<ProfileName, ProfileData*, ProfileNameHash>;
    ProfileMap m_Profiles;
    ProfileMap m_RegularProfiles;  // Store regular profiling data
    void PreserveProfilingState();
    void RestoreProfilingState();

    ~Profiler() {
        for (auto& [name, data] : m_Profiles) {
            m_DataPool.deallocate(data);
        }
    }

    /**
     * @brief Ensures the thread-local batched measurements container has sufficient capacity.
     *
     * This function checks whether the current capacity of the batched measurements vector is 
     * less than the predefined BATCH_RESERVE_SIZE. If it is, additional capacity is reserved to 
     * minimize the need for reallocations during batch profiling.
     */
    bool m_Enabled{true};
    std::unordered_set<std::string> m_StringPool;
    OutputFormat m_OutputFormat{OutputFormat::JSON};
    std::string m_JSONOutputPath{"profile_results.json"};
    bool m_HasUnsavedData{false};
    static bool s_SignalsInitialized;
    size_t m_MaxSamples{500};
    int m_Precision{4};
    mutable std::shared_mutex m_Mutex;
    
    struct BatchEntry {
        ProfileName name;
        float duration;
        
        BatchEntry(std::string_view n, float d) 
            : name(n), duration(d) {}
    };

    thread_local static std::vector<BatchEntry> m_BatchedMeasurements;
    size_t m_BatchSize = 0;
    bool m_HighPrecision = false;
    
    std::string_view InternString(std::string_view str);
    void FlushBatch();

    void ReserveBatch() {
        if (m_BatchedMeasurements.capacity() < BATCH_RESERVE_SIZE) {
            m_BatchedMeasurements.reserve(BATCH_RESERVE_SIZE);
        }
    }

    std::string m_CurrentSession;
    std::string m_RegularSession;  // Store original session name

    // Thread-local string buffer to avoid allocations
    struct ThreadLocalBuffer {
        char buffer[STRING_BUFFER_SIZE];
        size_t used{0};
        
        void reset() { used = 0; }
        
        char* allocate(size_t size) {
            if (used + size > STRING_BUFFER_SIZE) reset();
            char* result = buffer + used;
            used += size;
            return result;
        }
    };
    thread_local static ThreadLocalBuffer t_StringBuffer;

    // Fast path cache for frequently accessed profiles
    struct FastPathCache {
        struct Entry {
            ProfileName name{};
            ProfileData* data{nullptr};
            uint64_t lastAccess{0};
        };
        
        std::array<Entry, FAST_PATH_SIZE> entries;
        uint64_t accessCount{0};

        ProfileData* find(const ProfileName& name) {
            accessCount++;
            for (auto& entry : entries) {
                if (entry.data && entry.name == name) {
                    entry.lastAccess = accessCount;
                    return entry.data;
                }
            }
            return nullptr;
        }

        void insert(const ProfileName& name, ProfileData* data) {
            accessCount++;
            // Find least recently used entry
            size_t lru_idx = 0;
            uint64_t oldest = UINT64_MAX;
            for (size_t i = 0; i < FAST_PATH_SIZE; i++) {
                if (!entries[i].data) {
                    lru_idx = i;
                    break;
                }
                if (entries[i].lastAccess < oldest) {
                    oldest = entries[i].lastAccess;
                    lru_idx = i;
                }
            }
            // Ensure lru_idx is within bounds
            if (lru_idx < FAST_PATH_SIZE) {
                entries[lru_idx] = {name, data, accessCount};
            }
        }
    };
    thread_local static FastPathCache t_FastPath;

    static constexpr std::chrono::seconds WRITE_INTERVAL{5};
    std::chrono::steady_clock::time_point m_LastWriteTime{std::chrono::steady_clock::now()};

    bool m_ProfilingFrames{false};
    uint32_t m_FramesToProfile{0};
    uint32_t m_CurrentProfiledFrame{0};

    struct FrameData {
        uint32_t frameNumber;
        float frameTime;
        std::chrono::system_clock::time_point timestamp;
        
        FrameData(uint32_t num, float time) 
            : frameNumber(num), frameTime(time),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<FrameData> m_FrameData;
    std::string m_FramesJSONPath{"profiler_n_frames.json"};
    
    void WriteFrameDataJSON() const;  // New method declaration
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
