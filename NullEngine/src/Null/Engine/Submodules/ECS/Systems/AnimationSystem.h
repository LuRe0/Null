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
	struct AnimationComponent;

	class NLE_API AnimationSystem : public ISystem<AnimationSystem>
	{
	public:

		AnimationSystem();

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		
		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateAnimationComponent(void* component, const nlohmann::json& json);
		static void AddAnimationComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteAnimationComponent(const void* component);
		static JSON DiffAnimationComponent(const void* base, const void* modified);


		void ViewAnimationComponent(Entity& entity);

	};

}
