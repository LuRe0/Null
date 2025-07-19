#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	LifetimeSystem.h
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
	//class LifetimeComponent;

	class NLE_API LifetimeSystem : public ISystem<LifetimeSystem>
	{
	public:

		LifetimeSystem();

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;
		void RenderImGui() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;


		void RegisterToScripAPI(sol::state& lua) override;


	private:
		bool OnRuntimeStart(const EngineRunStateEvent& e);
		static void CreateLifetimeComponent(void* component, const nlohmann::json& json);

		static void AddLifetimeComponent(void* component, NRegistry* registry, EntityID id);

		static JSON WriteLifetimeComponent(const void* component);

		static JSON DiffLifetimeComponent(const void* base, const void* modified);


		void ViewLifetimeComponent(Entity& entity);


		std::set<std::string> m_Lifetimes;
	};

}
