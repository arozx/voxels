#pragma once

#include "Event.h"

namespace Engine {
    /**
     * @brief Base class for keyboard events
     */
    class KeyEvent : public Event {
    public:
        /** @return The key code of the event */
        int GetKeyCode() const { return m_KeyCode; }
        
        virtual int GetCategoryFlags() const override {
            return static_cast<int>(EventCategory::Keyboard) | 
                static_cast<int>(EventCategory::Input);
        }

    protected:
        /**
         * @brief Constructs a key event
         * @param keycode Code of the key involved
         */
        KeyEvent(int keycode) : m_KeyCode(keycode) {}
        int m_KeyCode;  ///< The key code
    };

    /**
     * @brief Event for key press with repeat detection
     */
    class KeyPressedEvent : public KeyEvent {
    public:
        /**
         * @brief Constructs a key pressed event
         * @param keycode Code of the pressed key
         * @param isRepeat Whether this is a repeat press
         */
        KeyPressedEvent(int keycode, bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        /** @return Whether this is a repeat press */
        bool IsRepeat() const { return m_IsRepeat; }

        virtual EventType GetEventType() const override { return EventType::KeyPressed; }
        virtual const char* GetName() const override { return "KeyPressed"; }

    private:
        bool m_IsRepeat;  ///< Whether this is a repeat press
    };

    /**
     * @brief Event for key release
     */
    class KeyReleasedEvent : public KeyEvent {
    public:
        /**
         * @brief Constructs a key released event
         * @param keycode Code of the released key
         */
        KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

        static EventType GetStaticType() { return EventType::KeyReleased; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "KeyReleased"; }
    };
}