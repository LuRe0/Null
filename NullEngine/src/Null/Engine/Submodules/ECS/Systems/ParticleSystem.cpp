
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
#include <glad/glad.h> 


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

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

		m_TotalMaxParticles = 0;
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

		InitParticleBuffer(GetSystemEntities(), registry);
	}

	void ParticleSystem::Update(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		m_ComputeShader->Bind();

		for (const auto entityId : GetSystemEntities())
		{
			ParticleSystemComponent& particleSystem = registry->GetComponent<ParticleSystemComponent>(entityId);

			for (ParticleEmitter& emitter : particleSystem.m_Emitters)
			{
				if (!emitter.enabled || emitter.finished)
					continue;

				// Set uniforms for the compute shader:
				m_ComputeShader->setFloat("u_DeltaTime", dt);
				m_ComputeShader->setVec3("u_EmitterPosition", emitter.position);
				m_ComputeShader->setVec3("u_EmitterAcceleration", emitter.acceleration);
				m_ComputeShader->setFloat("u_EmitterLifetime", emitter.lifetime);


				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ParticleSSBO);

				// Dispatch compute shader with number of work groups
				// Use ceil to cover all particles
				const GLuint workGroupSize = 256; // must match compute shader local_size_x
				GLuint groups = static_cast<GLuint>((emitter.maxParticles + workGroupSize - 1) / workGroupSize);

				// Provide emitter offset in SSBO as uniform
				m_ComputeShader->setInt("u_StartIndex", static_cast<int>(emitter.startIndex));

				m_ComputeShader->Dispatch(groups, 1, 1);
			}
		}

		m_ComputeShader->Unbind();
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

					emitter.startSize = emitterJsonWrapper.GetVec2("startSize", glm::vec2(1.0f, 0.0f));
					emitter.endSize = emitterJsonWrapper.GetVec2("endSize", glm::vec2(1.0f, 0.0f));
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
		nlohmann::json json;

		auto& psComp = *static_cast<ParticleSystemComponent*>(component);

		json["ParticleSystem"]["name"] = psComp.m_Name;

		// Emitters array
		nlohmann::json emittersJson = nlohmann::json::array();

		for (const auto& emitter : psComp.m_Emitters)
		{
			nlohmann::json e;

			e["name"] = emitter.name;
			e["emitterID"] = emitter.emitterID;

			e["position"] = { emitter.position.x, emitter.position.y, emitter.position.z };
			e["emissionRate"] = emitter.emissionRate;
			e["lifetime"] = emitter.lifetime;
			e["spawnTimer"] = emitter.spawnTimer;

			e["maxParticles"] = static_cast<int>(emitter.maxParticles);
			e["startIndex"] = static_cast<int>(emitter.startIndex);
			e["bufferOffset"] = static_cast<int>(emitter.bufferOffset);

			e["flags"] = emitter.flags;
			e["enabled"] = emitter.enabled;

			e["emitterDuration"] = emitter.emitterDuration;
			e["emitterAge"] = emitter.emitterAge;
			e["finished"] = emitter.finished;

			e["initialVelocity"] = { emitter.initialVelocity.x, emitter.initialVelocity.y, emitter.initialVelocity.z };
			e["acceleration"] = { emitter.acceleration.x, emitter.acceleration.y, emitter.acceleration.z };

			e["startSize"] = { emitter.startSize.x, emitter.startSize.y };
			e["endSize"] = { emitter.endSize.x, emitter.endSize.y };
			e["startColor"] = { emitter.startColor.r, emitter.startColor.g, emitter.startColor.b, emitter.startColor.a };
			e["endColor"] = { emitter.endColor.r, emitter.endColor.g, emitter.endColor.b, emitter.endColor.a };

			e["startRotation"] = emitter.startRotation;
			e["endRotation"] = emitter.endRotation;

			e["shape"] = static_cast<int>(emitter.shape);
			e["shapeSize"] = { emitter.shapeSize.x, emitter.shapeSize.y, emitter.shapeSize.z };

			e["burstCount"] = emitter.burstCount;

			emittersJson.push_back(e);
		}

		json["ParticleSystem"]["Emitters"] = emittersJson;

		return json;
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

		InitParticleBuffer(entityList, registry);

		return true;
	}

	void ParticleSystem::InitParticleBuffer(const std::vector<EntityID>& entityList, NRegistry* registry)
	{
		size_t totalNeeded = 0;
		for (const auto entityId : entityList)
		{
			const ParticleSystemComponent& psComp = registry->GetComponent<ParticleSystemComponent>(entityId);
			for (const ParticleEmitter& emitter : psComp.m_Emitters)
			{
				totalNeeded += emitter.maxParticles;
			}
		}

		if (totalNeeded > m_TotalMaxParticles)
		{
			m_TotalMaxParticles = totalNeeded;

			if (m_ParticleSSBO == 0)
				glGenBuffers(1, &m_ParticleSSBO);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ParticleSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, m_TotalMaxParticles * sizeof(ParticleInstance), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}