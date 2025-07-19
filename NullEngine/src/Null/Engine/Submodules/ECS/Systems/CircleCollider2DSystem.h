#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	CircleCollider2DSystem.h
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
	struct CircleCollider2DComponent;
	class NLE_API CircleCollider2DSystem : public ISystem<CameraSystem>
	{
	public:

		CircleCollider2DSystem();
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

		static void CreateCircleCollider2DComponent(void* component, const nlohmann::json& json);
		static void AddCircleCollider2DComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteCircleCollider2DComponent(const void* component);
		static JSON DiffCircleCollider2DComponent(const void* base, const void* modified);
		void ViewCircleCollider2DComponent(Entity& entityID);

		float m_Thickness = 0.075f;
		glm::vec4 m_Color = glm::vec4(1, 0.54, 0.31, 0.75);


		void CalculateOffset(glm::vec3& offset, Entity& entity);
		void CalculateOffset_rec(glm::vec3& offset, Entity& entity, Entity& parent, NSceneManager* sceneManager);

	};

}
