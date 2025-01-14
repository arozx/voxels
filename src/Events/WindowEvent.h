#pragma once
#include "Event.h"

namespace Engine {
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        virtual EventType GetEventType() const override { return EventType::WindowResize; }
        virtual const char* GetName() const override { return "WindowResize"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Application); 
        }

    private:
        unsigned int m_Width, m_Height;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "WindowClose"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Application); 
        }
    };
}