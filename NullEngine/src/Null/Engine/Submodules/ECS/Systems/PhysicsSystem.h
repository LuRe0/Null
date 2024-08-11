#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	PhysicsSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//

class b2World;

namespace NULLENGINE
{

	class BoxCollider2DComponent;
	class Rigidbody2DComponent;

	class NLE_API PhysicsSystem : public ISystem
	{
	public:

		PhysicsSystem();
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;
		void RenderImGui() override;
		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;
		
		// Conversion factor
		const float GetPixelPerMeter() { return m_Pixels_Per_Meter; } // 1 meter = 64 pixels

		const glm::vec2 MetersToPixels(float x, float y);
		const glm::vec2 PixelsToMeters(float x, float y);

	private:
		b2World* m_PhysicsWorld;

		static void CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static void CreateBoxCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		static JSON WriteRigidbody2DComponent(BaseComponent* component);

		static JSON WriteBoxCollider2DComponent(BaseComponent* component);


		void ViewRigidbody2DComponent(Entity& entityID);
		void ViewBoxCollider2DComponent(Entity& entityID);

		void InitializePhysics(EntityID entityID, NRegistry* registry);

		void OnEntityCreated(const EntityCreatedEvent& e);
		void OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		void OnEntityComponentAdded(const EntityAddComponentEvent& e);
		void OnSceneSwitched(const SceneSwitchEvent& e);

		const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81, 0.0f);
		// Conversion factor
		float m_Pixels_Per_Meter = 64.0f; // 1 meter = 64 pixels


		bool m_Simulate = false;
	};

}
