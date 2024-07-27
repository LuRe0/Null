#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	CommonEvents.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Null/Core.h"
#include "IEvents.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{


#define EVENT_CLASS_TYPE(type) static Event::EventType GetStaticEventType() { return EventType::##type; } \
                               virtual Event::EventType GetEventType() const override { return GetStaticEventType(); } \
                               virtual const char* GetName() const override { return #type; } \
                               std::unique_ptr<Event> Clone() const override  { return std::make_unique<type##Event>(*this); }

    class NLE_API WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width_, unsigned int height_)
            : m_Width(width_)
            , m_Height(height_)
        {
        }


        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: Width=" << m_Width << ", Height=" << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)

    private:
        unsigned int m_Width{ 0 };
        unsigned int m_Height{ 0 };
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
    };

    class WindowFocusEvent : public Event {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowFocus)
    };

    class WindowLostFocusEvent : public Event {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowLostFocus)
    };



    class KeyEvent : public Event {
    public:
        KeyEvent(int keycode) : m_Keycode(keycode) {}

        int GetKeyCode() const { return m_Keycode; }


    protected:
        int m_Keycode;
    };

    class KeyPressEvent : public KeyEvent {
    public:
        KeyPressEvent(int keycode, bool repeat)
            : KeyEvent(keycode), m_Repeat(repeat) {}

        EVENT_CLASS_TYPE(KeyPress)



        bool IsRepeat() const { return m_Repeat; }

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "KeyPressEvent: Key=" << m_Keycode << ", Repeat=" << (m_Repeat ? "True" : "False");
            return ss.str();
        }
    private:
        bool m_Repeat;
    };

    class KeyReleaseEvent : public KeyEvent {
    public:
        KeyReleaseEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "KeyReleaseEvent: Key=" << m_Keycode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyRelease)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "KeyReleaseEvent: Key=" << m_Keycode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyTyped)
    };

    class KeyHoldEvent : public KeyEvent {
    public:
        KeyHoldEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "KeyHoldEvent: Key=" << m_Keycode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyHold)
    };

    // Base class for all mouse button events
    class MouseButtonEvent : public Event {
    public:
        MouseButtonEvent(int button) : m_Button(button) {}

        int GetButton() const { return m_Button; }


    protected:
        int m_Button;
    };

    class MouseButtonPressEvent : public MouseButtonEvent {
    public:
        MouseButtonPressEvent(int button) : MouseButtonEvent(button) {}



        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "MouseButtonPressEvent: Button=" << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPress)
    };

    class MouseButtonReleaseEvent : public MouseButtonEvent {
    public:
        MouseButtonReleaseEvent(int button) : MouseButtonEvent(button) {}

        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "MouseButtonReleaseEvent: Button=" << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonRelease)
    };


    class MouseMoveEvent : public Event {
    public:
        MouseMoveEvent(float x, float y)
            : m_X(x), m_Y(y) {}

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }

        std::string Print() const override 
        {
            std::stringstream ss;
            ss << "MouseMoveEvent: X=" << m_X << ", Y=" << m_Y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMove)

    private:
        float m_X;
        float m_Y;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {}

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: XOffset=" << m_XOffset << ", YOffset=" << m_YOffset;
            return ss.str();
        }
        EVENT_CLASS_TYPE(MouseScrolled)

    private:
        float m_XOffset;
        float m_YOffset;
    };




    
     //Base class for all mouse button events
    class EntityModifiedEvent : public Event 
    {
    public:
        EntityModifiedEvent(uint32_t id, uint32_t componentID = 0) : m_EntityID(id), m_ComponentID(componentID) {}

        uint32_t GetID () const { return m_EntityID; }
        uint32_t GetComponentID () const { return m_ComponentID; }

	protected:
        uint32_t m_EntityID;
        uint32_t m_ComponentID;
	};


    // Base class for all mouse button events
    class EntityCreatedEvent : public EntityModifiedEvent {
    public:
        EntityCreatedEvent(uint32_t id) : EntityModifiedEvent(id) {}
        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntityCreatedEvent: ID=" << m_EntityID;
            return ss.str();
        }
        EVENT_CLASS_TYPE(EntityCreated)
    };

    class EntityDestroyedEvent : public EntityModifiedEvent {
    public:
        EntityDestroyedEvent(uint32_t id) : EntityModifiedEvent(id) {}
        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntityDestroyedEvent: ID=" << m_EntityID;
            return ss.str();
        }
        EVENT_CLASS_TYPE(EntityDestroyed)
    };

    // Base class for all mouse button events
    class EntityAddComponentEvent : public EntityModifiedEvent {
    public:
        EntityAddComponentEvent(uint32_t id, uint32_t compID) : EntityModifiedEvent(id, compID) {}
        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntityAddComponentEvent: ID=" << m_EntityID;
            return ss.str();
        }
        EVENT_CLASS_TYPE(EntityAddComponent)
    };

    class EntityRemoveComponentEvent : public EntityModifiedEvent {
    public:
        EntityRemoveComponentEvent(uint32_t id, uint32_t compID) : EntityModifiedEvent(id, compID) {}
        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntityRemoveComponentEvent: ID=" << m_EntityID;
            return ss.str();
        }
        EVENT_CLASS_TYPE(EntityRemoveComponent)
    };

}