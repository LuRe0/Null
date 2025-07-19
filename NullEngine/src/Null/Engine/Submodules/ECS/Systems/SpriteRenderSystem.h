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
	class NLE_API SpriteRenderSystem : public ISystem<SpriteRenderSystem>
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

		static void CreateSpriteComponent(void* component, const nlohmann::json& json);
		static void AddSpriteComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteSpriteComponent(const void* component);

		static JSON DiffSpriteComponent(const void* base, const void* modified);

		void ViewSpriteComponent(Entity& entityID);
		void DrawDragDrop(const char* label, uint32_t& nameID, SpriteSource*& source, NTextureManager* texMgr, NSpriteSourceManager* srcMgr);
	};

}
