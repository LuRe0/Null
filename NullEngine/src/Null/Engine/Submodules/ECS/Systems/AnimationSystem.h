#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	AnimationSystem.h
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
	class AnimationComponent;

	class NLE_API AnimationSystem : public ISystem
	{
	public:

		AnimationSystem();

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;


		AnimationComponent* Animation(Entity& entity);

		void Play(Entity& entity, bool state);
		void Pause(Entity& entity,  bool state);
		void Restart(Entity& entity,  bool state);
		void Stop(Entity& entity,  bool state);

		
		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateAnimationComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);
		static JSON WriteAnimationComponent(BaseComponent* component);


		void ViewAnimationComponent(Entity& entity);

	};

}
