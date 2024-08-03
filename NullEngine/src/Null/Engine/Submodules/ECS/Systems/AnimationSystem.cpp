
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

        NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

        // Register the create and view functions
        componentFactory->Register<AnimationComponent>(
            CreateAnimationComponent,  // Free function or static member function
            [this](Entity& entity) { this->ViewAnimationComponent(entity); },
            WriteAnimationComponent // Lambda capturing `this` for member function
        );
    }

    void AnimationSystem::Load()
	{
	}

	void AnimationSystem::Init()
	{
        ISystem::Init();

		//NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &AnimationSystem::OnWindowResize, eventManager);
	}

	void AnimationSystem::Update(float dt)
	{
        NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{
			AnimationComponent& anim = m_Parent->GetComponent<AnimationComponent>(entityId);
			SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);


            if (!anim.m_Enabled)
                continue;

            if (!anim.m_IsRunning || anim.m_IsDone) return;

            anim.m_FrameDelay -= dt;
            if (anim.m_FrameDelay <= 0.0f)
            {
                if (anim.m_IsReversed)
                {
                    if (anim.m_FrameIndex == 0)
                    {
                        if (anim.m_IsPingPong)
                        {
                            anim.m_IsReversed = false;
                            anim.m_FrameIndex = 1;
                        }
                        else if (anim.m_IsLooping)
                        {
                            anim.m_FrameIndex = anim.m_FrameCount - 1;
                        }
                        else
                        {
                            anim.m_IsDone = true;
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
                        if (anim.m_IsPingPong)
                        {
                            anim.m_IsReversed = true;
                            anim.m_FrameIndex = anim.m_FrameCount - 2;
                        }
                        else if (anim.m_IsLooping)
                        {
                            anim.m_FrameIndex = 0;
                        }
                        else
                        {
                            anim.m_IsDone = true;
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
        lua.new_usertype<NULLENGINE::AnimationComponent>(
            "AnimationComponent",
            // Expose members
            //"FrameIndex", &NULLENGINE::AnimationComponent::m_FrameIndex,
            "FrameCount", &NULLENGINE::AnimationComponent::m_FrameCount,
 //           "FrameOffset", &NULLENGINE::AnimationComponent::m_FrameOffset,
 //           "FrameDelay", &NULLENGINE::AnimationComponent::m_FrameDelay,
 //           "FrameDuration", &NULLENGINE::AnimationComponent::m_FrameDuration,
 //           "IsRunning", &NULLENGINE::AnimationComponent::m_IsRunning,
 ///*           "IsLooping", &NULLENGINE::AnimationComponent::m_IsLooping,
 //           "IsPingPong", &NULLENGINE::AnimationComponent::m_IsPingPong,*/
            "IsDone", &NULLENGINE::AnimationComponent::m_IsDone
            //"IsReversed", &NULLENGINE::AnimationComponent::m_IsReversed
        );
    }


    void AnimationSystem::CreateAnimationComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
    {
        NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

        auto* comp = static_cast<AnimationComponent*>(component);
        JsonReader jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {

            comp->m_FrameCount = jsonWrapper.GetInt("frameCount", 0);
            comp->m_FrameOffset = jsonWrapper.GetInt("startFrame", 0);
            comp->m_FrameDuration = jsonWrapper.GetFloat("frameDuration", 0.05f);
            comp->m_IsLooping = jsonWrapper.GetBool("loop", false);
            comp->m_IsPingPong = jsonWrapper.GetBool("pingPong", false);
            comp->m_IsReversed = jsonWrapper.GetBool("reverse", false);
            comp->m_IsRunning = jsonWrapper.GetBool("run", false);
        }

        componentFactory->AddOrUpdate<AnimationComponent>(id, comp, registry, comp->m_FrameCount, comp->m_FrameOffset, comp->m_FrameDuration, comp->m_IsLooping, comp->m_IsPingPong, comp->m_IsReversed, comp->m_IsRunning);

    }

    JSON AnimationSystem::WriteAnimationComponent(BaseComponent* component)
    {
        nlohmann::json json;

        auto& animation = *static_cast<AnimationComponent*>(component);

        json["Animation"]["startFrame"] = animation.m_FrameOffset;
        json["Animation"]["frameCount"] = animation.m_FrameCount;
        json["Animation"]["frameDuration"] = animation.m_FrameDuration;
        json["Animation"]["loop"] = animation.m_IsLooping;
        json["Animation"]["pingPong"] = animation.m_IsPingPong;
        json["Animation"]["reverse"] = animation.m_IsReversed;
        json["Animation"]["run"] = animation.m_IsRunning;

        return json;
    }

    void AnimationSystem::ViewAnimationComponent(Entity& entity)
    {
        AnimationComponent& animation = entity.Get<AnimationComponent>();

      
        ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(animation.m_FrameIndex)), 0.5f, 0);
        ImGui::DragInt("Frame Count", reinterpret_cast<int*>(&(animation.m_FrameCount)), 0.5f, 0);

        bool hasSprite = entity.Has<SpriteComponent>();
        if (hasSprite)
        {
            SpriteComponent& sprite = entity.Get<SpriteComponent>();
            if(sprite.m_SpriteSource)
              ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0, sprite.m_SpriteSource->GetFrameCount());
            
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0);
        }
        else
        {
            ImGui::DragInt("Frame Offset", reinterpret_cast<int*>(&(animation.m_FrameOffset)), 0.5f, 0);
        }

        ImGui::DragFloat("Frame Duration", &animation.m_FrameDuration, 0.5f, 0);
        
        ImGui::Checkbox("Loop", &animation.m_IsLooping);
        ImGui::Checkbox("Reverse", &animation.m_IsReversed);
        ImGui::Checkbox("Ping-Pong", &animation.m_IsPingPong);

        if(!hasSprite)
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Sprite component");

    }

}