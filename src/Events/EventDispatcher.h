#pragma once

#include <pch.h>
#include "Event.h"
#include "EventDebugger.h"

namespace Engine {
    struct EventCompare {
        bool operator()(const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) const {
            return static_cast<int>(a->GetPriority()) < static_cast<int>(b->GetPriority());
        }
    };

    class EventQueue {
    public:
        static EventQueue& Get() {
            static EventQueue instance;
            return instance;
        }

        void PushEvent(std::shared_ptr<Event> event) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            event->SetPriority(event->GetPriority());
            event->SetTimestamp(GetCurrentTimestamp());
            m_EventQueue.push(event);
            
            EventDebugger::Get().LogEvent(event);
        }

        bool PopEvent(std::shared_ptr<Event>& event) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            if (m_EventQueue.empty()) {
                return false;
            }
            event = m_EventQueue.top();
            m_EventQueue.pop();
            return true;
        }

        void Clear() {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::priority_queue<std::shared_ptr<Event>, 
                std::vector<std::shared_ptr<Event>>, 
                EventCompare> empty;
            std::swap(m_EventQueue, empty);
        }

        bool HasEvents() const {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            return !m_EventQueue.empty();
        }

    private:
        EventQueue() = default;
        uint64_t GetCurrentTimestamp() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
        }

        mutable std::mutex m_QueueMutex;
        std::priority_queue<std::shared_ptr<Event>, 
            std::vector<std::shared_ptr<Event>>, 
            EventCompare> m_EventQueue;
    };

    class EventDispatcher {
    public:
        using EventCallbackFn = std::function<bool(Event&)>;

        EventDispatcher(Event& event) : m_Event(event) {}

        template<typename T, typename F>
        bool Dispatch(const F& func) {
            if (m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.SetHandled(func(static_cast<T&>(m_Event)));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
}