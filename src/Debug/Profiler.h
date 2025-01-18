#pragma once
#include <pch.h>
#include <array>
#include <string_view>

namespace Engine {

// Ring buffer for storing samples with no allocations
template<typename T, size_t N>
class RingBuffer {
    std::array<T, N> buffer;
    std::atomic<size_t> head{0};
    std::atomic<size_t> count{0};
public:
    void push(T value) {
        size_t pos = head.fetch_add(1) % N;
        buffer[pos] = value;
        size_t curr_count = count.load();
        if (curr_count < N) count++;
    }
    
    std::vector<T> get_samples() const {
        std::vector<T> samples;
        size_t curr_count = std::min(count.load(), N);
        samples.reserve(curr_count);
        size_t curr_head = head.load() % N;
        for (size_t i = 0; i < curr_count; i++) {
            samples.push_back(buffer[(curr_head - i - 1) % N]);
        }
        return samples;
    }

    // Add standard container-like methods
    bool empty() const { return count == 0; }
    size_t size() const { return count; }
    
    // Add array-like access
    const T& operator[](size_t index) const {
        size_t curr_head = head.load();
        return buffer[(curr_head - index - 1) % N];
    }
    
    // Add iterator support
    class const_iterator {
        const RingBuffer* rb;
        size_t index;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const RingBuffer* rb_, size_t index_) : rb(rb_), index(index_) {}
        const T& operator*() const { return (*rb)[index]; }
        const_iterator& operator++() { ++index; return *this; }
        bool operator!=(const const_iterator& other) const { return index != other.index; }
        bool operator==(const const_iterator& other) const { return index == other.index; }
    };

    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const { return const_iterator(this, count); }
};

class ProfilerTimer {
public:
    ProfilerTimer(std::string_view name);
    ~ProfilerTimer();
private:
    std::string_view m_Name;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
};

class Profiler {
public:
    enum class OutputFormat {
        Console,
        JSON
    };

    static constexpr size_t RING_BUFFER_SIZE = 1000; // Moved before usage

    static Profiler& Get();
    
    void BeginSession(const std::string& name = "Profile");
    void EndSession();
    void WriteProfile(std::string_view name, float duration);
    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    void SetOutputFormat(OutputFormat format) { m_OutputFormat = format; }
    void SetJSONOutputPath(const std::string& filepath) { m_JSONOutputPath = filepath; }
    const std::unordered_map<std::string, RingBuffer<float, RING_BUFFER_SIZE>>& GetProfiles() const { return m_Profiles; }
    void ClearProfiles() { m_Profiles.clear(); }
    void Cleanup();
    static void InitSignalHandlers();
    void SetMaxSamples(size_t count) { m_MaxSamples = count; }
    void SetPrecision(int precision) { m_Precision = precision; }
    void SetSamplingRate(uint32_t n) { m_SampleEveryN = n; }
    void SetAsyncWrites(bool async) { m_AsyncWrites = async; }

private:
    Profiler();
    void WriteCompleteJSON() const;
    static void SignalHandler(int signal);
    void ProcessAsyncWrites();
    
    bool m_Enabled{true};
    std::string m_CurrentSession;
    std::unordered_map<std::string, RingBuffer<float, RING_BUFFER_SIZE>> m_Profiles;
    OutputFormat m_OutputFormat{OutputFormat::Console};
    std::string m_JSONOutputPath;
    bool m_HasUnsavedData{false};
    static bool s_SignalsInitialized;
    size_t m_MaxSamples{1000};       // Added back
    int m_Precision{3};              // Added back
    uint32_t m_SampleEveryN{1}; // Sample every Nth call
    std::atomic<uint32_t> m_CallCounter{0};
    bool m_AsyncWrites{true};
    
    struct WriteRequest {
        std::string name;
        float duration;
    };
    std::queue<WriteRequest> m_WriteQueue;
    std::mutex m_QueueMutex;
    std::condition_variable m_QueueCV;
    std::thread m_WriteThread;
    std::atomic<bool> m_StopThread{false};
};

}

#ifdef PROFILE_ENABLED
    #define PROFILE_SCOPE(name) Engine::ProfilerTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif
