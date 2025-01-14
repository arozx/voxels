#pragma once

#include <pch.h>

namespace Engine {
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
        AppTick, AppUpdate, AppRender
    };

    enum class EventCategory {
        None = 0,
        Application  = 1 << 0,
        Input       = 1 << 1,
        Keyboard    = 1 << 2,
        Mouse       = 1 << 3,
        MouseButton = 1 << 4
    };

    enum class EventPriority {
        Low = 0,
        Normal = 1,
        High = 2
    };

    class Event {
    public:
        virtual ~Event() = default;

        bool IsHandled() const { return m_Handled; }
        void SetHandled(bool handled) { m_Handled = handled; }

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }
        virtual EventPriority GetPriority() const { return m_Priority; }
        virtual void SetPriority(EventPriority priority) { m_Priority = priority; }

        bool IsInCategory(EventCategory category) const {
            return GetCategoryFlags() & static_cast<int>(category);
        }

        uint64_t GetTimestamp() const { return m_Timestamp; }
        void SetTimestamp(uint64_t timestamp) { m_Timestamp = timestamp; }
        virtual std::string GetDebugInfo() const { return ToString(); }

    protected:
        Event() : m_Timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) {}

        bool m_Handled = false;
        EventPriority m_Priority = EventPriority::Normal;
        uint64_t m_Timestamp;
    };
}