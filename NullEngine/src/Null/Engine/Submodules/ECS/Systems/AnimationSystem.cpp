
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
#include "NIncludes.h"

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
            DiffAnimationComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
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


            if (!anim.componentFlags.IsSet(ComponentFlags_Enabled))
                continue;

            if (!anim.flags.IsSet(AnimationFlags_IsRunning) || anim.flags.IsSet(AnimationFlags_IsDone)) return;

   
            anim.frameDelay -= dt;
            if (anim.frameDelay <= 0.0f)
            {
                if (anim.flags.IsSet(AnimationFlags_IsDone))
                {
                    if (anim.frameIndex == 0)
                    {
                        if (anim.flags.IsSet(AnimationFlags_IsPingPong))
                        {
                            anim.flags.Set(AnimationFlags_IsReversed);
                            anim.frameIndex = 1;
                        }
                        else if (anim.flags.IsSet(AnimationFlags_IsLooping))
                        {
                            anim.frameIndex = anim.frameCount - 1;
                        }
                        else
                        {
                            anim.flags.Set(AnimationFlags_IsDone);
                            anim.frameIndex = 0;
                        }
                    }
                    else
                    {
                        anim.frameIndex--;
                    }
                }
                else
                {
                    anim.frameIndex++;
                    if (anim.frameIndex >= anim.frameCount)
                    {
                        if (anim.flags.IsSet(AnimationFlags_IsPingPong))
                        {
                            anim.flags.Set(AnimationFlags_IsReversed);
                            anim.frameIndex = anim.frameCount - 2;
                        }
                        else if (anim.flags.IsSet(AnimationFlags_IsLooping))
                        {
                            anim.frameIndex = 0;
                        }
                        else
                        {
                            anim.flags.Set(AnimationFlags_IsDone);
                            anim.frameIndex = anim.frameCount - 1;
                        }
                    }
                }

                anim.frameDelay = anim.frameDuration; // Reset frame delay to the duration of the next frame

                sprite.frameIndex = anim.frameOffset + anim.frameIndex;
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
                { return anim.flags.IsSet(AnimationFlags_IsDone); },
                "set_looping", [](AnimationComponent& anim)
                {
                    anim.flags.Set(AnimationFlags_IsLooping);
                    anim.flags.Clear(AnimationFlags_IsPingPong);
                },
                "set_pingpong", [](AnimationComponent& anim)
                {
                    anim.flags.Clear(AnimationFlags_IsLooping);
                    anim.flags.Set(AnimationFlags_IsPingPong);
                },
                "set_reverse", [](AnimationComponent& anim)
                {
                    anim.flags.Set(AnimationFlags_IsReversed);
                },
                "play", [](AnimationComponent& anim)
                {
                    anim.flags.Set(AnimationFlags_IsRunning);
                },
                "pause", [](AnimationComponent& anim, float x, float y, float z)
                {
                    anim.flags.Clear(AnimationFlags_IsRunning);
                },
                "restart", [](AnimationComponent& anim)
                {
                    anim.flags.Clear(AnimationFlags_IsDone);

                    anim.flags.Set(AnimationFlags_IsRunning);

                    if (anim.flags.IsSet(AnimationFlags_IsReversed))
                    {
                        anim.frameIndex = anim.frameCount - 1;
                    }
                    else
                    {
                        anim.frameIndex = 0;
                    }
                },
                "stop", [](AnimationComponent& anim, float x, float y, float z)
                {
                    anim.flags.Set(AnimationFlags_IsDone);

                    anim.flags.Clear(AnimationFlags_IsRunning);

                    if (anim.flags.IsSet(AnimationFlags_IsReversed))
                    {
                        anim.frameIndex = anim.frameCount - 1;
                    }
                    else
                    {
                        anim.frameIndex = 0;
                    }
                }
        );
    }

    void AnimationSystem::PlayAnimation(Entity entity, int frameCount, float frameDuration, bool isLooping,
        unsigned int frameOffset, bool playReverse, bool pingPong, bool preserveFrame)
    {
        if (!entity.Has<AnimationComponent>())
            return;

        AnimationComponent& anim = entity.Get<AnimationComponent>();

        // Set animation parameters
        anim.frameCount = frameCount;
        anim.frameOffset = frameOffset;
        anim.frameDuration = frameDuration;

        // Set flags
        anim.flags.Set(AnimationFlags_IsLooping, isLooping);
        anim.flags.Set(AnimationFlags_IsReversed, playReverse);
        anim.flags.Set(AnimationFlags_IsPingPong, pingPong);
        anim.flags.Set(AnimationFlags_IsRunning, true);
        anim.flags.Set(AnimationFlags_IsDone, false);

        // Handle frame position based on preserve flag
        if (preserveFrame)
        {
            // Switch behavior - preserve current frame if valid
            if (anim.frameIndex >= frameCount)
            {
                anim.frameIndex = 0;
            }
            // Don't reset frameDelay - continue with current timing
        }
        else
        {
            // Play behavior - restart from beginning
            anim.frameIndex = 0;
            anim.frameDelay = frameDuration;
        }

        // Update sprite if it exists
        if (entity.Has<SpriteComponent>())
        {
            SpriteComponent& sprite = entity.Get<SpriteComponent>();
            sprite.frameIndex = anim.frameOffset + anim.frameIndex;
        }
    }

    void AnimationSystem::PlayAnimation(Entity entity, int frameCount, float frameDuration, bool isLooping,
        unsigned int frameOffset, bool playReverse, bool pingPong)
    {
        PlayAnimation(entity, frameCount, frameDuration, isLooping, frameOffset, playReverse, pingPong, false);
    }

    void AnimationSystem::SwitchAnimation(Entity entity, int frameCount, float frameDuration, bool isLooping,
        unsigned int frameOffset, bool playReverse, bool pingPong)
    {
        PlayAnimation(entity, frameCount, frameDuration, isLooping, frameOffset, playReverse, pingPong, true);
    }

    void AnimationSystem::CreateAnimationComponent(void* component, const nlohmann::json& json)
    {
        NComponentFactory* componentFactory = NComponentFactory::Instance();

        auto* comp = static_cast<AnimationComponent*>(component);
        JsonReader jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {

            comp->frameCount = jsonWrapper.GetInt("frameCount", 0);
            comp->frameOffset = jsonWrapper.GetInt("startFrame", 0);
            comp->frameDuration = jsonWrapper.GetFloat("frameDuration", 0.05f);

			AnimationFlagSet animFlags;

			comp->flags.m_Flags = jsonWrapper.GetUInt8("AnimationFlags", 0);


            ComponentFlagSet flags;
            flags.Set(ComponentFlags_Enabled);
            flags.Set(ComponentFlags_Serialized);

            comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

        }

        //componentFactory->AddOrUpdate<AnimationComponent>(id, comp, registry, comp->m_FrameCount, comp->m_FrameOffset,
        //    comp->m_FrameDuration, comp->m_IsLooping, comp->m_IsPingPong, comp->m_IsReversed, comp->m_IsRunning);

    }

    void AnimationSystem::AddAnimationComponent(void* component, NRegistry* registry, EntityID id)
    {
        NComponentFactory* componentFactory = NComponentFactory::Instance();

        auto* comp = static_cast<AnimationComponent*>(component);

        componentFactory->AddOrUpdate<AnimationComponent>(id, comp, registry, comp->frameIndex, comp->frameCount, 
            comp->frameOffset, comp->frameDelay, comp->frameDuration, comp->componentFlags, comp->flags);

    }

    JSON AnimationSystem::WriteAnimationComponent(const void* component)
    {
        nlohmann::json json;

        auto& animation = *static_cast<const AnimationComponent*>(component);


        if(!animation.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

        json["Animation"]["startFrame"] = animation.frameOffset;
        json["Animation"]["frameCount"] = animation.frameCount;
        json["Animation"]["frameDuration"] = animation.frameDuration;
        json["Animation"]["ComponentFlags"] = animation.componentFlags.m_Flags;
        json["Animation"]["AnimationFlags"] = animation.flags.m_Flags;

        return json;
    }

    JSON AnimationSystem::DiffAnimationComponent(const void* base, const void* modified)
    {
        const AnimationComponent* a = static_cast<const AnimationComponent*>(base);
        const AnimationComponent* b = static_cast<const AnimationComponent*>(modified);

        JSON diff;
        JSON animJson;

        if (a->frameOffset != b->frameOffset)
            animJson["startFrame"] = b->frameOffset;

        if (a->frameCount != b->frameCount)
            animJson["frameCount"] = b->frameCount;

        if (a->frameDuration != b->frameDuration)
            animJson["frameDuration"] = b->frameDuration;

        if (a->componentFlags.m_Flags != b->componentFlags.m_Flags)
        {
			animJson["ComponentFlags"] = b->componentFlags.m_Flags;
        }

        if( a->flags.m_Flags != b->flags.m_Flags)
        {
            animJson["AnimationFlags"] = b->flags.m_Flags;
		}


        if (!animJson.empty())
            diff["Animation"] = animJson;

        return diff;
    }


    void AnimationSystem::ViewAnimationComponent(Entity& entity)
    {
        AnimationComponent& animation = entity.Get<AnimationComponent>();


        uint8_t& flags = animation.componentFlags.m_Flags;
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

      
        ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(animation.frameIndex)), 0.5f, 0);
        ImGui::DragInt("Frame Count", reinterpret_cast<int*>(&(animation.frameCount)), 0.5f, 0);

        bool hasSprite = entity.Has<SpriteComponent>();
        if (hasSprite)
        {
            SpriteComponent& sprite = entity.Get<SpriteComponent>();
            if (sprite.spriteSourceNameID)
            {
				auto* spriteSource = NSpriteSourceManager::Instance()->Get(sprite.spriteSourceNameID);
                ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.frameOffset)), 0.5f, 0, spriteSource->GetFrameCount());
            }
            
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.frameOffset)), 0.5f, 0);
        }
        else
        {
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.frameOffset)), 0.5f, 0);
        }

        ImGui::DragFloat("Frame Duration", &animation.frameDuration, 0.5f, 0);
        
        bool isLooping = animation.flags.IsSet(AnimationFlags_IsLooping);
        if (ImGui::Checkbox("Loop", &isLooping))
            animation.flags.Set(AnimationFlags_IsLooping, isLooping);

        bool isReversed = animation.flags.IsSet(AnimationFlags_IsReversed);
        if (ImGui::Checkbox("Reverse", &isReversed))
            animation.flags.Set(AnimationFlags_IsReversed, isReversed);

        bool isPingPong = animation.flags.IsSet(AnimationFlags_IsPingPong);
        if (ImGui::Checkbox("Ping-Pong", &isPingPong))
            animation.flags.Set(AnimationFlags_IsPingPong, isPingPong);

        if(!hasSprite)
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Sprite component");


        if (!enabled)
            ImGui::EndDisabled();




        ImGui::TreePop();

    }

}