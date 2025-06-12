#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.h
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
	struct ParticleSystemComponent;
	class NLE_API ParticleSystem : public ISystem
	{
	public:

		ParticleSystem();
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

		static void CreateParticleSystemComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteParticleSystemComponent(BaseComponent* component);

		void ViewParticleSystemComponent(Entity& entityID);

		bool OnEntityCreated(const EntityCreatedEvent& e);

		void InitTilemap(const std::vector<EntityID>& entityList, NRegistry* registry);
	};

}
