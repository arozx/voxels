#pragma once

#include "Event.h"

namespace Engine {
    /**
     * @brief Event triggered when window is resized
     */
    class WindowResizeEvent : public Event {
    public:
        /**
         * @brief Constructs a window resize event
         * @param width New window width
         * @param height New window height
         */
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        /** @return Current window width */
        unsigned int GetWidth() const { return m_Width; }
        /** @return Current window height */
        unsigned int GetHeight() const { return m_Height; }

        virtual EventType GetEventType() const override { return EventType::WindowResize; }
        virtual const char* GetName() const override { return "WindowResize"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Application); 
        }

    private:
        unsigned int m_Width, m_Height;  ///< Window dimensions
    };

    /**
     * @brief Event triggered when window is closed
     */
    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        /** @return Static type identifier for this event */
        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "WindowClose"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Application); 
        }
    };
}