#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
//#include "Null/Engine/Submodules/ECS/Systems/System.h"
//#include  "Null/Engine/NEngine.h"
#include "Null/Engine/Submodules/Graphics/Shader/ComputeShader.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/BatchRenderer.h"



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
	struct ParticleSystemComponent;
	struct ParticleEmitter;
	class NLE_API ParticleSystem : public ISystem<ParticleSystem>
	{
	public:

		ParticleSystem();
		void Load() override;
		//! Virtual Init function
		void Init() override;
		void SetEmitterUniforms(ComputeShader& shader, const ParticleEmitter& emitter, const TransformComponent& transform);
		//! Virtual Update function
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void RenderImGui() override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;




		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateParticleSystemComponent(void* component, const nlohmann::json& json);
		static void AddParticleSystemComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteParticleSystemComponent(const void* component);

		static JSON DiffParticleSystemComponent(const void* base, const void* modified);

		void DrawCoreSettings(ParticleEmitter& emitter, size_t i);

		void DrawColorConfig(ParticleEmitter& emitter, size_t i);

		void DrawLifetimeSettings(ParticleEmitter& emitter, size_t i);

		void DrawPhysicsConfig(ParticleEmitter& emitter, size_t i);

		void DrawInitConfig(ParticleEmitter& emitter, size_t i);

		void DrawForcesConfig(ParticleEmitter& emitter, size_t i);

		void DrawSizeConfig(ParticleEmitter& emitter, size_t i);

		void DrawRotationConfig(ParticleEmitter& emitter, size_t i);

		void DrawFadeConfig(ParticleEmitter& emitter, size_t i);

		void DrawAnimationConfig(ParticleEmitter& emitter, size_t i);

		void DrawShapeConfig(ParticleEmitter& emitter, size_t i);

		void ViewParticleSystemComponent(Entity& entityID);

		bool OnEntityCreated(const EntityCreatedEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);
		bool OnEntityDestroyed(const EntityDestroyedEvent& e);
		bool OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		bool OnEntityComponentAdded(const EntityAddComponentEvent& e);

		void InitParticleBuffer(const std::vector<EntityID>& entityList, NRegistry* registry);
		void RemoveFromParticleBuffer(EntityID entity, NRegistry* registry);

		ComputeShader* m_UpdateComputeShader;
		ComputeShader* m_EmitComputeShader;
		ComputeShader* m_InitComputeShader;

		SSBO m_ParticleSSBO;
		size_t m_TotalMaxParticles = 0;  // total particle count for all emitters combined
		unsigned int atomicCounterBufferID;
		BatchRenderer* m_Batcher;
	};

}
