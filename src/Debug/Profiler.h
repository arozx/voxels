#pragma once
#include "pch.h"

namespace Engine {

class ProfilerTimer {
public:
    ProfilerTimer(const std::string& name);
    ~ProfilerTimer();
private:
    std::string m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};

class Profiler {
public:
    static Profiler& Get();
    
    void BeginSession(const std::string& name = "Profile");
    void EndSession();
    
    void WriteProfile(const std::string& name, float duration);
    
    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    
    const std::unordered_map<std::string, std::vector<float>>& GetProfiles() const { return m_Profiles; }
    void ClearProfiles() { m_Profiles.clear(); }

private:
    Profiler() = default;
    bool m_Enabled = true;
    std::string m_CurrentSession;
    std::unordered_map<std::string, std::vector<float>> m_Profiles;
};

}

// Profiling Macros
#define PROFILE_SCOPE(name) Engine::ProfilerTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
