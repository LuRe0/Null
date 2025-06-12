
//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ParticleSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include "Null/Engine/Submodules/Events/IEvents.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	ParticleSystem::ParticleSystem()
	{
		Require<TransformComponent>();
		Require<ParticleSystemComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<ParticleSystemComponent>(CreateParticleSystemComponent,
			[this](Entity& id) { this->ViewParticleSystemComponent(id); }, WriteParticleSystemComponent);
	}

	void ParticleSystem::Load()
	{
		ISystem::Load();
	}

	void ParticleSystem::Init()
	{
		ISystem::Init();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &ParticleSystem::OnEntityCreated, eventManager, EventPriority::Low);

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		InitTilemap(GetSystemEntities(), registry);
	}

	void ParticleSystem::Update(float dt)
	{

	}

	void ParticleSystem::Render()
	{
	}

	void ParticleSystem::Unload()
	{

	}

	void ParticleSystem::Shutdown()
	{
	}

	void ParticleSystem::RegisterToScripAPI(sol::state& lua)
	{

	}


	void ParticleSystem::CreateParticleSystemComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		auto* comp = static_cast<ParticleSystemComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Name = jsonWrapper.GetString("name", "");

			if (json.contains("Emitters") && json["Emitters"].is_array())
			{
				for (const auto& emitterJson : json["Emitters"])
				{

					JsonReader emitterJsonWrapper(emitterJson);

					ParticleEmitter emitter;

					emitter.name = emitterJsonWrapper.GetString("name", "");
					emitter.emitterID = emitterJsonWrapper.GetInt("emitterID", 0);

					emitter.position = emitterJsonWrapper.GetVec3("position", glm::vec3(0.0f));
					emitter.emissionRate = emitterJsonWrapper.GetFloat("emissionRate", 10.0f);

					emitter.lifetime = emitterJsonWrapper.GetFloat("lifetime", 5.0f);
					emitter.spawnTimer = emitterJsonWrapper.GetFloat("spawnTimer", 0.0f);

					emitter.maxParticles = emitterJsonWrapper.GetInt("maxParticles", 1000);

					emitter.flags = emitterJsonWrapper.GetInt("flags", PARTICLE_FLAG_NONE);
					emitter.enabled = emitterJsonWrapper.GetBool("enabled", true);

					emitter.emitterDuration = emitterJsonWrapper.GetFloat("emitterDuration", -1.0f);

					emitter.initialVelocity = emitterJsonWrapper.GetVec3("initialVelocity", glm::vec3(0.0f));
					emitter.acceleration = emitterJsonWrapper.GetVec3("acceleration", glm::vec3(0.0f));

					emitter.startSize = emitterJsonWrapper.GetVec2("sizeRange", glm::vec2(1.0f, 0.0f));
					emitter.endSize = emitterJsonWrapper.GetVec2("sizeRange", glm::vec2(1.0f, 0.0f));
					emitter.startColor = emitterJsonWrapper.GetVec4("startColor", glm::vec4(1.0f));
					emitter.endColor = emitterJsonWrapper.GetVec4("endColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

					emitter.startRotation = emitterJsonWrapper.GetFloat("startRotation", 0.0f);
					emitter.endRotation = emitterJsonWrapper.GetFloat("endRotation", 0.0f);

					emitter.shape = static_cast<SpawnShape>(emitterJsonWrapper.GetInt("shape", static_cast<int>(SpawnShape::Point)));
					emitter.shapeSize = emitterJsonWrapper.GetVec3("shapeSize", glm::vec3(1.0f));

					emitter.burstCount = emitterJsonWrapper.GetFloat("burstCount", 0.0f);

					comp->m_Emitters.push_back(emitter);
				}
			}
		}


		// Add or update component in registry
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		componentFactory->AddOrUpdate<ParticleSystemComponent>(id, comp, registry, comp->m_Name);
	}


	JSON ParticleSystem::WriteParticleSystemComponent(BaseComponent* component)
	{

		return JSON();
	}

	void ParticleSystem::ViewParticleSystemComponent(Entity& entity)
	{
	}

	bool ParticleSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
			return true;

		InitTilemap(entityList, registry);

		return true;
	}

	void ParticleSystem::InitTilemap(const std::vector<EntityID>& entityList, NRegistry* registry)
	{
		for (const auto entityId : entityList)
		{
			ParticleSystemComponent& tilemapComp = registry->GetComponent<ParticleSystemComponent>(entityId);
		}

	}
}