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

	private:
		std::unique_ptr<InstanceMesh> m_InstanceMesh;

		static void CreateSpriteComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		void ViewSpriteComponent(Entity& entityID);
	};

}
