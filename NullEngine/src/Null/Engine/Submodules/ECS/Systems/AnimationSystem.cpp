
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




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

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

        componentFactory->Register<AnimationComponent>(CreateAnimationComponent);
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


    void AnimationSystem::CreateAnimationComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
    {
        NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

        auto* comp = static_cast<AnimationComponent*>(component);
        JsonWrapper jsonWrapper(json);


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

}