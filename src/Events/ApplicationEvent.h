#pragma once

#include "Event.h"

namespace Engine {
    /**
     * @brief Event triggered when window is resized
     * 
     * Provides information about the new window dimensions.
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

        /** @return Static type identifier for this event */
        static EventType GetStaticType() { return EventType::WindowResize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "WindowResize"; }
        virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

    private:
        unsigned int m_Width, m_Height;  ///< Window dimensions
    };

    /**
     * @brief Event triggered when window is closed
     * 
     * Signals that the window is being closed and the application should terminate.
     */
    class WindowCloseEvent : public Event {
    public:
        /** @brief Constructs a window close event */
        WindowCloseEvent() {}

        /** @return Static type identifier for this event */
        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "WindowClose"; }
        virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
    };
}