#pragma once

#include "Event.h"
#include <pch.h>

namespace Engine {
    class EventDebugger {
    public:
        /**
         * @brief Debug information for an event instance
         */
        struct EventDebugInfo {
            std::string name;      ///< Event name
            EventPriority priority; ///< Event priority
            bool handled;          ///< Whether event was handled
            uint64_t timestamp;    ///< When event occurred
            std::string debugInfo; ///< Additional debug information
            float timeAgo = 0.0f;  ///< Time since event occurred
        };

        /** @return Reference to singleton instance */
        static EventDebugger& Get() {
            static EventDebugger instance;
            return instance;
        }

        /**
         * @brief Records an event for debugging
         * @param event Event to log
         */
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

        /** @return List of recent events */
        const std::deque<EventDebugInfo>& GetEventHistory() const { return m_EventHistory; }
        
        /** @brief Clears event history */
        void Clear() { 
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_EventHistory.clear(); 
        }

        /**
         * @brief Updates time information for logged events
         * @param deltaTime Time elapsed since last update
         */
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