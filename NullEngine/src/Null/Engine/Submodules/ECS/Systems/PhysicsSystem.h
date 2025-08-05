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
class b2Body;
class b2Fixture;
struct b2FixtureDef;
struct b2BodyDef;
struct b2Vec2;

namespace NULLENGINE
{

	struct BoxCollider2DComponent;
	struct Rigidbody2DComponent;
	struct TransformComponent;
	class NRegistry;

	class NLE_API PhysicsSystem : public ISystem<PhysicsSystem>
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
		//static const b2Vec2 PixelsToB2Meters(float x, float y);

		static const glm::vec2 PixelsToMeters(const glm::vec2& size);

		static const float MetersToPixels(float meters);
		static const float PixelsToMeters(float pixels);

		static void LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation);
		static void LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
		static void WorldToLocalPos(TransformComponent& transform, TransformComponent& parentTransform);

		size_t AddActiveBody(b2Body* body);	
		size_t AddActiveFixture(b2Fixture* fixture);
		void UpdateActiveFixture(b2Fixture* fixture, uint32_t index);

		void RemoveActiveBody(uint32_t index);
		void RemoveActiveFixture(uint32_t index, b2Body* body);

		b2Body* GetActiveBody(uint32_t index) const;
		b2Fixture* GetActiveFixture(uint32_t index) const;

		b2Body* CreateBody(const b2BodyDef& bodyDef);
		b2Fixture* CreateFixture(b2Body* body, const b2FixtureDef& fixtureDef);

		//void RegisterB2Body(b2Body* body);
	private:
		b2World* m_PhysicsWorld;

		std::vector<b2Body*> m_ActiveBodies;
		std::vector<b2Fixture*> m_ActiveFixtures;
		std::vector<uint32_t> m_FreeFixtureIDs;
		std::vector<uint32_t> m_FreeBodyIDs;


	/*	uint32_t CreateFixture(b2Body* body, const b2FixtureDef& fixtureDef);
		

		uint32_t CreateBody(const Rigidbody2DComponent& rb2d, EntityID entID);*/

		static void CreateRigidbody2DComponent(void* component, const nlohmann::json& json);
		static void AddRigidbody2DComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteRigidbody2DComponent(const void* component);
		static JSON DiffRigidbody2DComponent(const void* base, const void* modified);
		void ViewRigidbody2DComponent(Entity& entityID);


		bool InitializePhysics(EntityID entityID, NRegistry* registry);
		bool DestroyPhysics(EntityID entityID, NRegistry* registry);


		//bool HasRequiredComponents(NRegistry* registry, EntityID entityID);
		//bool HandleParents(NRegistry* registry, EntityID entityID);
		bool OnEntityCreated(const EntityCreatedEvent& e);
		bool OnEntityDestroyed(const EntityDestroyedEvent& e);
		bool OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		//bool OnEntityParented(const EntityParentedEvent& e);
		//bool OnEntitySeparated(const EntitySeparatedEvent& e);
		bool OnEntityComponentAdded(const EntityAddComponentEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);
		bool OnSceneStart(const InitializeBox2DEvent& e);


		float m_Thickness = 0.55f;
		glm::vec4 m_Color = glm::vec4(0, 0, 1, 1);

		const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81, 0.0f);
		// Conversion factor
		static float m_Pixels_Per_Meter; // 1 meter = 64 pixels


		bool m_Simulate = false;

		//ContactListener m_ContactListener;
	};

}
