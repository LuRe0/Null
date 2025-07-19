
//------------------------------------------------------------------------------
//
// File Name:	AnimationSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "AnimationSystem.h"
#include "imgui.h"
#include "../Entities/Entity.h"
#include "../../../../Tools/ImGuiH.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    AnimationSystem::AnimationSystem()
    {
        Require<AnimationComponent>();
        Require<SpriteComponent>();

        NComponentFactory* componentFactory = NComponentFactory::Instance();

        // Register the create and view functions
        componentFactory->Register<AnimationComponent>(
            CreateAnimationComponent,  // Free function or static member function
            [this](Entity& entity) { this->ViewAnimationComponent(entity); },
            WriteAnimationComponent, // Lambda capturing `this` for member function,
            AddAnimationComponent, // Lambda capturing `this` for member function,
            DiffAnimationComponent
        );
    }

    void AnimationSystem::Load()
	{
	}

	void AnimationSystem::Init()
	{
        ISystem::Init();

		//NEventManager* eventManager =   NEventManager::Instance();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &AnimationSystem::OnWindowResize, eventManager);
	}

	void AnimationSystem::Update(float dt)
	{

	}

    void AnimationSystem::RuntimeUpdate(float dt)
    {
        NRegistry* m_Parent = NRegistry::Instance();

        for (const auto entityId : GetSystemEntities())
        {
            AnimationComponent& anim = m_Parent->GetComponent<AnimationComponent>(entityId);
            SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);


            if (!anim.m_ComponentFlags.IsSet(ComponentFlags_Enabled))
                continue;

            if (!anim.m_Flags.IsSet(AnimationFlags_IsRunning) || anim.m_Flags.IsSet(AnimationFlags_IsDone)) return;

   
            anim.m_FrameDelay -= dt;
            if (anim.m_FrameDelay <= 0.0f)
            {
                if (anim.m_Flags.IsSet(AnimationFlags_IsDone))
                {
                    if (anim.m_FrameIndex == 0)
                    {
                        if (anim.m_Flags.IsSet(AnimationFlags_IsPingPong))
                        {
                            anim.m_Flags.Set(AnimationFlags_IsReversed);
                            anim.m_FrameIndex = 1;
                        }
                        else if (anim.m_Flags.IsSet(AnimationFlags_IsLooping))
                        {
                            anim.m_FrameIndex = anim.m_FrameCount - 1;
                        }
                        else
                        {
                            anim.m_Flags.Set(AnimationFlags_IsDone);
                            anim.m_FrameIndex = 0;
                        }
                    }
                    else
                    {
                        anim.m_FrameIndex--;
                    }
                }
                else
                {
                    anim.m_FrameIndex++;
                    if (anim.m_FrameIndex >= anim.m_FrameCount)
                    {
                        if (anim.m_Flags.IsSet(AnimationFlags_IsPingPong))
                        {
                            anim.m_Flags.Set(AnimationFlags_IsReversed);
                            anim.m_FrameIndex = anim.m_FrameCount - 2;
                        }
                        else if (anim.m_Flags.IsSet(AnimationFlags_IsLooping))
                        {
                            anim.m_FrameIndex = 0;
                        }
                        else
                        {
                            anim.m_Flags.Set(AnimationFlags_IsDone);
                            anim.m_FrameIndex = anim.m_FrameCount - 1;
                        }
                    }
                }

                anim.m_FrameDelay = anim.m_FrameDuration; // Reset frame delay to the duration of the next frame

                sprite.m_FrameIndex = anim.m_FrameOffset + anim.m_FrameIndex;
            }
        }
    }


	void AnimationSystem::Render()
	{
	}

	void AnimationSystem::Unload()
	{
	}

	void AnimationSystem::Shutdown()
	{
	}


    void AnimationSystem::RegisterToScripAPI(sol::state& lua)
    {
        lua.new_usertype<AnimationComponent>
            (
                "Animation",
                sol::no_constructor,
                "type_id", &Component<AnimationComponent>::GetID,
                "is_done", [](AnimationComponent& anim)
                { return anim.m_Flags.IsSet(AnimationFlags_IsDone); },
                "set_looping", [](AnimationComponent& anim)
                {
                    anim.m_Flags.Set(AnimationFlags_IsLooping);
                    anim.m_Flags.Clear(AnimationFlags_IsPingPong);
                },
                "set_pingpong", [](AnimationComponent& anim)
                {
                    anim.m_Flags.Clear(AnimationFlags_IsLooping);
                    anim.m_Flags.Set(AnimationFlags_IsPingPong);
                },
                "set_reverse", [](AnimationComponent& anim)
                {
                    anim.m_Flags.Set(AnimationFlags_IsReversed);
                },
                "play", [](AnimationComponent& anim)
                {
                    anim.m_Flags.Set(AnimationFlags_IsRunning);
                },
                "pause", [](AnimationComponent& anim, float x, float y, float z)
                {
                    anim.m_Flags.Clear(AnimationFlags_IsRunning);
                },
                "restart", [](AnimationComponent& anim)
                {
                    anim.m_Flags.Clear(AnimationFlags_IsDone);

                    anim.m_Flags.Set(AnimationFlags_IsRunning);

                    if (anim.m_Flags.IsSet(AnimationFlags_IsReversed))
                    {
                        anim.m_FrameIndex = anim.m_FrameCount - 1;
                    }
                    else
                    {
                        anim.m_FrameIndex = 0;
                    }
                },
                "stop", [](AnimationComponent& anim, float x, float y, float z)
                {
                    anim.m_Flags.Set(AnimationFlags_IsDone);

                    anim.m_Flags.Clear(AnimationFlags_IsRunning);

                    if (anim.m_Flags.IsSet(AnimationFlags_IsReversed))
                    {
                        anim.m_FrameIndex = anim.m_FrameCount - 1;
                    }
                    else
                    {
                        anim.m_FrameIndex = 0;
                    }
                }
        );
    }


    void AnimationSystem::CreateAnimationComponent(void* component, const nlohmann::json& json)
    {
        NComponentFactory* componentFactory = NComponentFactory::Instance();

        auto* comp = static_cast<AnimationComponent*>(component);
        JsonReader jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {

            comp->m_FrameCount = jsonWrapper.GetInt("frameCount", 0);
            comp->m_FrameOffset = jsonWrapper.GetInt("startFrame", 0);
            comp->m_FrameDuration = jsonWrapper.GetFloat("frameDuration", 0.05f);

			AnimationFlagSet animFlags;

			comp->m_Flags.m_Flags = jsonWrapper.GetUInt8("AnimationFlags", 0);


            ComponentFlagSet flags;
            flags.Set(ComponentFlags_Enabled);
            flags.Set(ComponentFlags_Serialized);

            comp->m_ComponentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

        }

        //componentFactory->AddOrUpdate<AnimationComponent>(id, comp, registry, comp->m_FrameCount, comp->m_FrameOffset,
        //    comp->m_FrameDuration, comp->m_IsLooping, comp->m_IsPingPong, comp->m_IsReversed, comp->m_IsRunning);

    }

    void AnimationSystem::AddAnimationComponent(void* component, NRegistry* registry, EntityID id)
    {
        NComponentFactory* componentFactory = NComponentFactory::Instance();

        auto* comp = static_cast<AnimationComponent*>(component);

        componentFactory->AddOrUpdate<AnimationComponent>(id, comp, registry, comp->m_FrameIndex, comp->m_FrameCount, 
            comp->m_FrameOffset, comp->m_FrameDelay, comp->m_FrameDuration, comp->m_ComponentFlags, comp->m_Flags);

    }

    JSON AnimationSystem::WriteAnimationComponent(const void* component)
    {
        nlohmann::json json;

        auto& animation = *static_cast<const AnimationComponent*>(component);

        json["Animation"]["startFrame"] = animation.m_FrameOffset;
        json["Animation"]["frameCount"] = animation.m_FrameCount;
        json["Animation"]["frameDuration"] = animation.m_FrameDuration;
        json["Animation"]["ComponentFlags"] = animation.m_ComponentFlags.m_Flags;
        json["Animation"]["AnimationFlags"] = animation.m_Flags.m_Flags;

        return json;
    }

    JSON AnimationSystem::DiffAnimationComponent(const void* base, const void* modified)
    {
        const AnimationComponent* a = static_cast<const AnimationComponent*>(base);
        const AnimationComponent* b = static_cast<const AnimationComponent*>(modified);

        JSON diff;
        JSON animJson;

        if (a->m_FrameOffset != b->m_FrameOffset)
            animJson["startFrame"] = b->m_FrameOffset;

        if (a->m_FrameCount != b->m_FrameCount)
            animJson["frameCount"] = b->m_FrameCount;

        if (a->m_FrameDuration != b->m_FrameDuration)
            animJson["frameDuration"] = b->m_FrameDuration;

        if (a->m_ComponentFlags.m_Flags != b->m_ComponentFlags.m_Flags)
        {
			animJson["ComponentFlags"] = b->m_ComponentFlags.m_Flags;
        }

        if( a->m_Flags.m_Flags != b->m_Flags.m_Flags)
        {
            animJson["AnimationFlags"] = b->m_Flags.m_Flags;
		}


        if (!animJson.empty())
            diff["Animation"] = animJson;

        return diff;
    }


    void AnimationSystem::ViewAnimationComponent(Entity& entity)
    {
        AnimationComponent& animation = entity.Get<AnimationComponent>();


        uint8_t& flags = animation.m_ComponentFlags.m_Flags;
        // Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
        auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Animation", flags, ComponentFlags_Enabled);

        if (remove)
        {
            NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<AnimationComponent>::GetID()));
        }

        if (!open)
            return;

        if (!enabled)
            ImGui::BeginDisabled();

      
        ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(animation.m_FrameIndex)), 0.5f, 0);
        ImGui::DragInt("Frame Count", reinterpret_cast<int*>(&(animation.m_FrameCount)), 0.5f, 0);

        bool hasSprite = entity.Has<SpriteComponent>();
        if (hasSprite)
        {
            SpriteComponent& sprite = entity.Get<SpriteComponent>();
            if (sprite.m_SpriteSourceNameID)
            {
				auto* spriteSource = NSpriteSourceManager::Instance()->Get(sprite.m_SpriteSourceNameID);
                ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0, spriteSource->GetFrameCount());
            }
            
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0);
        }
        else
        {
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0);
        }

        ImGui::DragFloat("Frame Duration", &animation.m_FrameDuration, 0.5f, 0);
        
        bool isLooping = animation.m_Flags.IsSet(AnimationFlags_IsLooping);
        if (ImGui::Checkbox("Loop", &isLooping))
            animation.m_Flags.Set(AnimationFlags_IsLooping, isLooping);

        bool isReversed = animation.m_Flags.IsSet(AnimationFlags_IsReversed);
        if (ImGui::Checkbox("Reverse", &isReversed))
            animation.m_Flags.Set(AnimationFlags_IsReversed, isReversed);

        bool isPingPong = animation.m_Flags.IsSet(AnimationFlags_IsPingPong);
        if (ImGui::Checkbox("Ping-Pong", &isPingPong))
            animation.m_Flags.Set(AnimationFlags_IsPingPong, isPingPong);

        if(!hasSprite)
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Sprite component");


        if (!enabled)
            ImGui::EndDisabled();




        ImGui::TreePop();

    }

}