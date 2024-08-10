#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	SpriteRenderSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"
#include "Null/Engine/Submodules/Graphics/InstanceMesh.h"


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
	class SpriteComponent;
	class NLE_API SpriteRenderSystem : public ISystem
	{
	public:

		SpriteRenderSystem();
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;




		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateSpriteComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteSpriteComponent(BaseComponent* component);

		void ViewSpriteComponent(Entity& entityID);
	};

}
