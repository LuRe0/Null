#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	TagSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"
#include "Null/Engine/Submodules/Graphics/Camera/Shake/CameraShake.h"


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
	//class TagComponent;

	class NLE_API TagSystem : public ISystem
	{
	public:

		TagSystem();

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
		static void CreateTagComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		static JSON WriteTagComponent(BaseComponent* component);


		void ViewTagComponent(Entity& entity);


		std::set<std::string> m_tags;
	};

}
