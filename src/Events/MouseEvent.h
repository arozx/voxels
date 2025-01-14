#pragma once

#include "Event.h"

namespace Engine {
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        virtual EventType GetEventType() const override { return EventType::MouseMoved; }
        virtual const char* GetName() const override { return "MouseMoved"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input); 
        }

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseButtonEvent : public Event {
    public:
        int GetButton() const { return m_Button; }
        
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input) | 
                static_cast<int>(EventCategory::MouseButton); 
        }

    protected:
        MouseButtonEvent(int button) : m_Button(button) {}
        int m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        virtual EventType GetEventType() const override { return EventType::MouseButtonPressed; }
        virtual const char* GetName() const override { return "MouseButtonPressed"; }
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        virtual EventType GetEventType() const override { return EventType::MouseButtonReleased; }
        virtual const char* GetName() const override { return "MouseButtonReleased"; }
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {}

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        virtual EventType GetEventType() const override { return EventType::MouseScrolled; }
        virtual const char* GetName() const override { return "MouseScrolled"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input); 
        }

    private:
        float m_XOffset, m_YOffset;
    };
}