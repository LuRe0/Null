
//------------------------------------------------------------------------------
//
// File Name:	LifetimeSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "LifetimeSystem.h"
#include "Null/Engine/Submodules/ECS/Components/LifetimeComponent.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Entities/Entity.h"
#include "../../Scene.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	LifetimeSystem::LifetimeSystem()
	{
		Require<LifetimeComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<LifetimeComponent>(CreateLifetimeComponent,
			[this](Entity& id) { this->ViewLifetimeComponent(id); }, WriteLifetimeComponent);

	}


	void LifetimeSystem::Load()
	{
	}

	void LifetimeSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager =   NEventManager::Instance();


		//SUBSCRIBE_EVENT(EngineRunStateEvent, &LifetimeSystem::OnRuntimeStart, eventManager, EventPriority::Low);
	}

	void LifetimeSystem::Update(float dt)
	{

	}

	void LifetimeSystem::RuntimeUpdate(float dt)
	{
		NRegistry* registry = NRegistry::Instance();
		

		for (const auto entityId : GetSystemEntities())
		{
			LifetimeComponent& LifetimeComp = registry->GetComponent<LifetimeComponent>(entityId);
			LifetimeComp.timeRemaining -= dt;

			if (LifetimeComp.timeRemaining <= 0.0f) 
			{
				NSceneManager* scMan = NSceneManager::Instance();
				Scene* sc = scMan->GetCurrentScene();
				if(sc->HasEntity(entityId))
					sc->GetEntity(entityId).m_isDestroyed = true;
			}
		}
	}


	void LifetimeSystem::Render()
	{
	}

	void LifetimeSystem::RenderImGui()
	{
		ISystem::RenderImGui();
	}

	void LifetimeSystem::Unload()
	{
	}

	void LifetimeSystem::Shutdown()
	{
	}

	void LifetimeSystem::RegisterToScripAPI(sol::state& lua)
	{

	}


	bool LifetimeSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{

		return false;
	}

	void LifetimeSystem::CreateLifetimeComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		auto* comp = static_cast<LifetimeComponent*>(component);
		JsonReader jsonWrapper(json);
		if (!jsonWrapper.Empty())
		{
			comp->timeRemaining = jsonWrapper.GetFloat("timeRemaining", 1.0f);
		}

		componentFactory->AddOrUpdate<LifetimeComponent>(id, comp, registry, comp->timeRemaining);
	}

	JSON LifetimeSystem::WriteLifetimeComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<LifetimeComponent*>(component);
		json["Lifetime"]["timeRemaining"] = comp.timeRemaining;


		return json;
	}
	void LifetimeSystem::ViewLifetimeComponent(Entity& entity)
	{
		if (!entity.Has<LifetimeComponent>())
			return;

		auto& LifetimeComp = entity.Get<LifetimeComponent>();

		ImGui::DragFloat("Lifetime", &LifetimeComp.timeRemaining, 0.001f, 0);

	}


}