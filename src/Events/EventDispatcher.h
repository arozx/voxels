#pragma once

#include <pch.h>
#include "Event.h"
#include "../Threading/ThreadPool.h"

namespace Engine {
    /**
     * @brief Comparator for event priority queue ordering
     */
    struct EventCompare {
        bool operator()(const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) const {
            return static_cast<int>(a->GetPriority()) < static_cast<int>(b->GetPriority());
        }
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

    /**
     * @brief Thread-safe event queue for storing and processing events
     * 
     * Implements a priority queue for events with thread-safe access
     */
    class EventQueue {
    public:
        using EventHandlerFn = std::function<bool(std::shared_ptr<Event>)>;

        /** @return Reference to singleton instance */
        static EventQueue& Get() {
            static EventQueue instance;
            return instance;
        }

        /**
         * @brief Adds an event to the queue
         * @param event Event to be queued
         */
        void PushEvent(std::shared_ptr<Event> event) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            event->SetPriority(event->GetPriority());
            event->SetTimestamp(GetCurrentTimestamp());
            m_EventQueue.push(event);
            
            EventDebugger::Get().LogEvent(event);
        }

        /**
         * @brief Removes and returns the next event
         * @param[out] event Pointer to store the retrieved event
         * @return true if an event was retrieved
         */
        bool PopEvent(std::shared_ptr<Event>& event) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            if (m_EventQueue.empty()) {
                return false;
            }
            event = m_EventQueue.top();
            m_EventQueue.pop();
            return true;
        }

        /** @brief Empties the event queue */
        void Clear() {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::priority_queue<std::shared_ptr<Event>, 
                std::vector<std::shared_ptr<Event>>, 
                EventCompare> empty;
            std::swap(m_EventQueue, empty);
        }

        /** @return true if queue contains events */
        bool HasEvents() const {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            return !m_EventQueue.empty();
        }

        /**
         * @brief Processes all events in the queue
         * @param handler Event handler function
         */
        void ProcessEvents(EventHandlerFn handler) {
            while (HasEvents()) {
                std::shared_ptr<Event> event;
                if (PopEvent(event)) {
                    handler(event);
                }
            }
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
}