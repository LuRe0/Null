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
//#include "Null/Engine/Submodules/ECS/Entities/Entity.h"

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


    class Entity;
    class EntityLoadedEvent : public Event
    {
    public:
        EntityLoadedEvent(const Entity& ent) : m_Entity(ent)
        {}

        std::string Print() const override;

        const Entity& GetEntity() const { return m_Entity; }

        EVENT_CLASS_TYPE(EntityLoaded)

    private:
        const Entity& m_Entity;
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

    class SceneSwitchEvent : public Event {
    public:
        SceneSwitchEvent(const std::string& from, const std::string& to)
        : m_CurrentScene(from)
        , m_NextScene(to) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "SceneSwitchEvent: from: " << m_CurrentScene << ", to: " << m_NextScene;
            return ss.str();
        }

        const std::string& GetCurrentScene() const { return m_CurrentScene; }
        const std::string& GetNextScene() const { return m_NextScene; }

        EVENT_CLASS_TYPE(SceneSwitch)

    private:
        const std::string m_CurrentScene;
        const std::string m_NextScene;
    };


    class SceneLoadedEvent : public Event {
    public:
        SceneLoadedEvent(const std::string& from, const std::string& to)
            : m_CurrentScene(from)
            , m_NextScene(to) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "SceneLoadedEvent: from: " << m_CurrentScene << ", to: " << m_NextScene;
            return ss.str();
        }

        const std::string& GetCurrentScene() const { return m_CurrentScene; }
        const std::string& GetNextScene() const { return m_NextScene; }

        EVENT_CLASS_TYPE(SceneLoaded)

    private:
        const std::string m_CurrentScene;
        const std::string m_NextScene;
    };


    class ScriptCreatedEvent : public Event 
    {
    public:
        ScriptCreatedEvent(uint32_t eID, const std::string script)
            : m_EntityID(eID)
            , m_Scriptname(script) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "ScriptCreatedEvent: Entity: " << m_EntityID << ", Script: " << m_Scriptname;
            return ss.str();
        }

        const uint32_t& GetEntityID() const { return m_EntityID; }
        const std::string& GetScriptName() const { return m_Scriptname; }

        EVENT_CLASS_TYPE(ScriptCreated)

    private:
        const uint32_t m_EntityID;
        const std::string m_Scriptname;
  
    };


    class ScriptRemovedEvent : public Event
    {
    public:
        ScriptRemovedEvent(uint32_t eID, const std::string& script)
            : m_EntityID(eID)
            , m_Scriptname(script) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "ScriptRemovedEvent: Entity: " << m_EntityID << ", Script: " << m_Scriptname;
            return ss.str();
        }

        const uint32_t& GetEntityID() const { return m_EntityID; }
        const std::string& GetScriptName() const { return m_Scriptname; }

        EVENT_CLASS_TYPE(ScriptRemoved)

    private:
        const uint32_t m_EntityID;
        const std::string m_Scriptname;

    };


    class ScriptModifiedEvent : public Event
    {
    public:
        ScriptModifiedEvent(const std::string script)
            : m_Scriptname(script) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "ScriptModifiedEvent: script:" << m_Scriptname;
            return ss.str();
        }

        const std::string& GetScriptName() const { return m_Scriptname; }

        EVENT_CLASS_TYPE(ScriptModified)

    private:
        const std::string m_Scriptname;

    };


    class EngineStateEvent : public Event {
    public:
        EngineStateEvent(IEngine::EngineState enginestate) : m_State(enginestate) {}

        IEngine::EngineState GetState() const { return m_State; }


    protected:
        IEngine::EngineState m_State;
    };


    class EngineRunStateEvent : public EngineStateEvent {
    public:
        EngineRunStateEvent(IEngine::EngineState enginestate)
            : EngineStateEvent(enginestate) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "KeyReleaseEvent: State = RUN";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EngineRunState)
    };

    class EngineEditStateEvent : public EngineStateEvent {
    public:
        EngineEditStateEvent(IEngine::EngineState enginestate)
            : EngineStateEvent(enginestate) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "KeyReleaseEvent: State = EDIT";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EngineEditState)
    };

    class EnginePauseStateEvent : public EngineStateEvent {
    public:
        EnginePauseStateEvent(IEngine::EngineState enginestate)
            : EngineStateEvent(enginestate) {}

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "KeyReleaseEvent: State = EDIT";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EnginePauseState)
    };



    class InitializeBox2DEvent : public Event
    {
    public:
        InitializeBox2DEvent(uint32_t eID)
            : m_EntityID(eID)
        {}


        const uint32_t& GetEntityID() const { return m_EntityID; }


        std::string Print() const override
        {
            std::stringstream ss;
            ss << "InitializeBox2DEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(InitializeBox2D)

    private:
        const uint32_t m_EntityID;
    };


    class EntityParentedEvent : public Event
    {
    public:
        EntityParentedEvent(uint32_t parentID, uint32_t childID)
            : m_ParentID(parentID), m_ChildID(childID)
        {}


        const uint32_t& GetParentID() const { return m_ParentID; }
        const uint32_t& GetChildID() const { return m_ChildID; }


        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntityParentedEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EntityParented)

    private:
        const uint32_t m_ParentID;
        const uint32_t m_ChildID;
    };


    class EntitySeparatedEvent : public Event
    {
    public:
        EntitySeparatedEvent(uint32_t parentID, uint32_t childID)
            : m_ParentID(parentID), m_ChildID(childID)
        {}


        const uint32_t& GetParentID() const { return m_ParentID; }
        const uint32_t& GetChildID() const { return m_ChildID; }


        std::string Print() const override
        {
            std::stringstream ss;
            ss << "EntitySeparatedEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EntitySeparated)

    private:
        const uint32_t m_ParentID;
        const uint32_t m_ChildID;
    };



    class CollisionOccuredEvent : public Event
    {
    public:
        CollisionOccuredEvent(uint32_t oA, uint32_t oB)
            : m_EntityA(oA), m_EntityB(oB)
        {}


        const uint32_t& GetEntityA() const { return m_EntityA; }
        const uint32_t& GetEntityB() const { return m_EntityB; }
    private:
        const uint32_t m_EntityA;
        const uint32_t m_EntityB;
    };


    class CollisionEnterEvent : public CollisionOccuredEvent
    {
    public:
        CollisionEnterEvent(uint32_t oA, uint32_t oB)
            : CollisionOccuredEvent(oA, oB)
        {

        }

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "CollisionEnterEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(CollisionEnter)
    };

    class CollisionExitEvent : public CollisionOccuredEvent
    {
    public:
        CollisionExitEvent(uint32_t oA, uint32_t oB)
            : CollisionOccuredEvent(oA, oB)
        {

        }

        std::string Print() const override
        {
            std::stringstream ss;
            ss << "CollisionExitEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(CollisionExit)
    };

}