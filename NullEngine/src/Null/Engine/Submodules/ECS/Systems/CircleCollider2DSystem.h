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
	class CircleCollider2DComponent;
	class NLE_API CircleCollider2DSystem : public ISystem
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

		static void CreateCircleCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteCircleCollider2DComponent(BaseComponent* component);

		float m_Thickness = 0.075f;
		glm::vec4 m_Color = glm::vec4(1, 0.54, 0.31, 0.75);

		void ViewCircleCollider2DComponent(Entity& entityID);
	};

}
