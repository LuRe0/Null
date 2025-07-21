#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	BoxCollider2DSystem.h
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


namespace NULLENGINE
{
	struct SpriteComponent;
	class NLE_API BoxCollider2DSystem : public ISystem<BoxCollider2DSystem>
	{
	public:

		BoxCollider2DSystem();
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		void RenderImGui() override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;




		void RegisterToScripAPI(sol::state& lua) override;

	private:

		float m_Thickness = 0.55f;
		glm::vec4 m_Color = glm::vec4(0, 1, 0, 0.5);

		static JSON WriteBoxCollider2DComponent(const void* component);
		static JSON DiffBoxCollider2DComponent(const void* base, const void* modified);
		static void CreateBoxCollider2DComponent(void* component, const nlohmann::json& json);
		static void AddBoxCollider2DComponent(void* component, NRegistry* registry, EntityID id);
		void ViewBoxCollider2DComponent(Entity& entityID);
		bool InitializeCollider(EntityID entityID, NRegistry* registry);


		bool OnEntityCreated(const EntityCreatedEvent& e);
		bool OnEntityDestroyed(const EntityDestroyedEvent& e);
		bool OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		bool OnEntityComponentAdded(const EntityAddComponentEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);
		bool OnInitializeBox2DStart(const InitializeBox2DColliderEvent& e);

		void CalculateOffset(glm::vec3& offset, Entity& entity);
		void AddCollider(BoxCollider2DComponent& collider, Entity& ent);
		void CalculateOffset_rec(glm::vec3& offset, Entity& entity, Entity& parent, NSceneManager* sceneManager);
	};

}
