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
	class TransformComponent;
	class NRegistry;

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

		static const glm::vec2 MetersToPixels(float x, float y);
		static const glm::vec2 PixelsToMeters(float x, float y);

		static const float MetersToPixels(float meters);
		static const float PixelsToMeters(float pixels);

		static void LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation);
		static void LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
		static void WorldToLocalPos(TransformComponent& transform, TransformComponent& parentTransform);
	private:
		b2World* m_PhysicsWorld;

		static void CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteRigidbody2DComponent(BaseComponent* component);
		void ViewRigidbody2DComponent(Entity& entityID);


		bool InitializePhysics(EntityID entityID, NRegistry* registry);
		bool InitializeColliders(EntityID entityID, NRegistry* registry, Rigidbody2DComponent& rb2d);
		bool InitializeChildrenColliders(EntityID entityID, NRegistry* registry, Rigidbody2DComponent& rb2d, TransformComponent& parentTransform);
		//bool HasRequiredComponents(NRegistry* registry, EntityID entityID);
		//bool HandleParents(NRegistry* registry, EntityID entityID);
		bool OnEntityCreated(const EntityCreatedEvent& e);
		bool OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		bool OnEntityParented(const EntityParentedEvent& e);
		bool OnEntitySeparated(const EntitySeparatedEvent& e);
		bool OnEntityComponentAdded(const EntityAddComponentEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);
		bool OnSceneStart(const InitializeBox2DEvent& e);



		void HandleParenting_Rec(NSceneManager* sceneManager, Entity& parent, Entity& child);
		void HandleSeparation_Rec(NSceneManager* sceneManager, Entity& parent, Entity& child);
		float m_Thickness = 0.55f;
		glm::vec4 m_Color = glm::vec4(0, 0, 1, 1);

		const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81, 0.0f);
		// Conversion factor
		static float m_Pixels_Per_Meter; // 1 meter = 64 pixels


		bool m_Simulate = false;
	};

}
