#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	TilemapRenderSystem.h
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
	struct TilemapComponent;
	class NLE_API TilemapRenderSystem : public ISystem
	{
	public:

		TilemapRenderSystem();
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

		static void CreateTilemapComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteTilemapComponent(BaseComponent* component);

		void ViewTilemapComponent(Entity& entityID);

		bool OnEntityCreated(const EntityCreatedEvent& e);

		void InitTilemap(const std::vector<EntityID>& entityList, NRegistry* registry);
	};

}
