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

	class NLE_API TagSystem : public ISystem<TagSystem>
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
		static void CreateTagComponent(void* component, const nlohmann::json& json);

		static void AddTagComponent(void* component, NRegistry* registry, EntityID id);

		static JSON WriteTagComponent(const void* component);

		static JSON DiffTagComponent(const void* base, const void* modified);


		void ViewTagComponent(Entity& entity);


		std::set<uint32_t> m_tags;
	};

}
