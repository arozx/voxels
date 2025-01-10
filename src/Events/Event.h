#pragma once

#include <string>
#include <functional>

namespace Engine {
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput      = 1 << 1,
        EventCategoryKeyboard   = 1 << 2,
        EventCategoryMouse      = 1 << 3,
        EventCategoryMouseButton = 1 << 4
    };

    class Event {
    public:
        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) {
            return GetCategoryFlags() & category;
        }

        bool IsHandled() const { return m_Handled; }
        void SetHandled(bool handled) { m_Handled = handled; }

    protected:
        bool m_Handled = false;
    };

    using EventCallbackFn = std::function<void(Event&)>;

    class EventDispatcher {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        template<typename T>
        bool Dispatch(const std::function<bool(T&)>& func) {
            if (m_Event.GetEventType() == T::GetStaticType()) {
                bool handled = func(static_cast<T&>(m_Event));
                if (handled) {
                    m_Event.SetHandled(true);
                }
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
}