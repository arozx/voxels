#pragma once

#include <pch.h>

namespace Engine {
    /**
     * @brief Enumeration of all possible event types in the system
     */
    enum class EventType {
        None = 0,
        WindowClose,      ///< Window close event
        WindowResize,     ///< Window resize event
        WindowFocus,      ///< Window focus gained event
        WindowLostFocus,  ///< Window focus lost event
        WindowMoved,      ///< Window moved event
        KeyPressed,       ///< Keyboard key pressed event
        KeyReleased,      ///< Keyboard key released event
        KeyTyped,        ///< Keyboard key typed event
        MouseButtonPressed,   ///< Mouse button pressed event
        MouseButtonReleased,  ///< Mouse button released event
        MouseMoved,          ///< Mouse moved event
        MouseScrolled        ///< Mouse wheel scrolled event
    };

    /**
     * @brief Event category flags for filtering events
     */
    enum EventCategory {
        None = 0,
        EventCategoryApplication = (1 << 0),  ///< Application events
        EventCategoryInput       = (1 << 1),  ///< Input events
        EventCategoryKeyboard   = (1 << 2),  ///< Keyboard events
        EventCategoryMouse      = (1 << 3),  ///< Mouse events
        EventCategoryMouseButton = (1 << 4)  ///< Mouse button events
    };

    /**
     * @brief Base class for all events in the system
     */
    class Event {
    public:
        virtual ~Event() = default;
        
        /** @return The type of this event */
        virtual EventType GetEventType() const = 0;
        
        /** @return Human-readable name of the event */
        virtual const char* GetName() const = 0;
        
        /** @return Category flags for this event */
        virtual int GetCategoryFlags() const = 0;
        
        /** @return String representation of the event */
        virtual std::string ToString() const { return GetName(); }

        /**
         * @brief Checks if event belongs to a category
         * @param category Category to check
         * @return true if event is in category
         */
        bool IsInCategory(EventCategory category);

    protected:
        bool m_Handled = false;  ///< Whether event has been handled
    };
}