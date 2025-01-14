#pragma once

#include "Event.h"

namespace Engine {
    /**
     * @brief Event for mouse cursor movement
     */
    class MouseMovedEvent : public Event {
    public:
        /**
         * @brief Constructs a mouse moved event
         * @param x New X coordinate
         * @param y New Y coordinate
         */
        MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

        /** @return Current X coordinate */
        float GetX() const { return m_MouseX; }
        /** @return Current Y coordinate */
        float GetY() const { return m_MouseY; }

        virtual EventType GetEventType() const override { return EventType::MouseMoved; }
        virtual const char* GetName() const override { return "MouseMoved"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input); 
        }

    private:
        float m_MouseX, m_MouseY;  ///< Mouse coordinates
    };

    /**
     * @brief Base class for mouse button events
     */
    class MouseButtonEvent : public Event {
    public:
        /** @return The button involved in the event */
        int GetButton() const { return m_Button; }
        
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input) | 
                static_cast<int>(EventCategory::MouseButton); 
        }

    protected:
        MouseButtonEvent(int button) : m_Button(button) {}
        int m_Button;  ///< The button code
    };

    /**
     * @brief Event for mouse button press
     */
    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        /**
         * @brief Constructs a mouse button pressed event
         * @param button Code of the pressed button
         */
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        virtual EventType GetEventType() const override { return EventType::MouseButtonPressed; }
        virtual const char* GetName() const override { return "MouseButtonPressed"; }
    };

    /**
     * @brief Event for mouse button release
     */
    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        /**
         * @brief Constructs a mouse button released event
         * @param button Code of the released button
         */
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        virtual EventType GetEventType() const override { return EventType::MouseButtonReleased; }
        virtual const char* GetName() const override { return "MouseButtonReleased"; }
    };

    /**
     * @brief Event for mouse wheel scrolling
     */
    class MouseScrolledEvent : public Event {
    public:
        /**
         * @brief Constructs a mouse scrolled event
         * @param xOffset Horizontal scroll amount
         * @param yOffset Vertical scroll amount
         */
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {}

        /** @return Horizontal scroll amount */
        float GetXOffset() const { return m_XOffset; }
        /** @return Vertical scroll amount */
        float GetYOffset() const { return m_YOffset; }

        virtual EventType GetEventType() const override { return EventType::MouseScrolled; }
        virtual const char* GetName() const override { return "MouseScrolled"; }
        virtual int GetCategoryFlags() const override { 
            return static_cast<int>(EventCategory::Mouse) | 
                static_cast<int>(EventCategory::Input); 
        }

    private:
        float m_XOffset, m_YOffset;  ///< Scroll amounts
    };
}