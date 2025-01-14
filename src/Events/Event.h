#pragma once

#include <pch.h>

namespace Engine {
    /**
     * @brief Enumeration of all possible event types in the system
     */
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
        AppTick, AppUpdate, AppRender
    };

    /**
     * @brief Categories for event filtering and routing
     */
    enum class EventCategory {
        None = 0,
        Application  = 1 << 0,
        Input       = 1 << 1,
        Keyboard    = 1 << 2,
        Mouse       = 1 << 3,
        MouseButton = 1 << 4
    };

    /**
     * @brief Priority levels for event processing
     */
    enum class EventPriority {
        Low = 0,     ///< Low priority events
        Normal = 1,  ///< Normal priority events
        High = 2     ///< High priority events
    };

    /**
     * @brief Base class for all event types
     * 
     * Provides common functionality for event handling, identification,
     * and categorization. All specific event types derive from this class.
     */
    class Event {
    public:
        virtual ~Event() = default;

        /** @return Whether the event has been handled */
        bool IsHandled() const { return m_Handled; }
        /** @param handled New handled state */
        void SetHandled(bool handled) { m_Handled = handled; }

        /** @return Type identifier for this event */
        virtual EventType GetEventType() const = 0;
        /** @return Human-readable name of the event */
        virtual const char* GetName() const = 0;
        /** @return Bit flags of categories this event belongs to */
        virtual int GetCategoryFlags() const = 0;
        /** @return String representation of the event */
        virtual std::string ToString() const { return GetName(); }
        /** @return Priority level of this event */
        virtual EventPriority GetPriority() const { return m_Priority; }
        /** @param priority New priority level */
        virtual void SetPriority(EventPriority priority) { m_Priority = priority; }

        /**
         * @brief Checks if event belongs to a category
         * @param category Category to check against
         * @return true if event is in the category
         */
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