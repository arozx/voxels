#pragma once
#include "Event.h"

namespace Engine {
    class KeyEvent : public Event {
    public:
        int GetKeyCode() const { return m_KeyCode; }
        
        virtual int GetCategoryFlags() const override {
            return EventCategoryKeyboard | EventCategoryInput;
        }

    protected:
        KeyEvent(int keycode) : m_KeyCode(keycode) {}
        int m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        bool IsRepeat() const { return m_IsRepeat; }

        virtual EventType GetEventType() const override { return EventType::KeyPressed; }
        virtual const char* GetName() const override { return "KeyPressed"; }

    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

        static EventType GetStaticType() { return EventType::KeyReleased; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "KeyReleased"; }
    };
}