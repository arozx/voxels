#pragma once

#include "Event.h"
#include <pch.h>


namespace Engine {
    class EventDebugger {
    public:
        struct EventDebugInfo {
            std::string name;
            EventPriority priority;
            bool handled;
            uint64_t timestamp;
            std::string debugInfo;
            float timeAgo = 0.0f;
        };

        static EventDebugger& Get() {
            static EventDebugger instance;
            return instance;
        }

        void LogEvent(const std::shared_ptr<Event>& event) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_EventHistory.push_front(EventDebugInfo{
                event->GetName(),
                event->GetPriority(),
                event->IsHandled(),
                event->GetTimestamp(),
                event->GetDebugInfo(),
                0.0f
            });

            // Keep only most recent 5 events
            if (m_EventHistory.size() > 5) {
                m_EventHistory.pop_back();
            }
        }

        const std::deque<EventDebugInfo>& GetEventHistory() const { return m_EventHistory; }
        void Clear() { 
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_EventHistory.clear(); 
        }

        void UpdateTimestamps(float deltaTime) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            for (auto& event : m_EventHistory) {
                event.timeAgo += deltaTime;
            }
        }

    private:
        EventDebugger() = default;
        std::deque<EventDebugInfo> m_EventHistory;
        mutable std::mutex m_Mutex;
        const size_t m_MaxEventHistory = 5;
    };
}