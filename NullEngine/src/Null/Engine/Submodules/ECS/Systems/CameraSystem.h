#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	CameraSystem.h
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
	//class CameraComponent;

	class NLE_API CameraSystem : public ISystem
	{
	public:

		CameraSystem();

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
		//void OnWindowResize(const WindowResizeEvent& e);
		static void CreateCameraComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		static JSON WriteCameraComponent(BaseComponent* component);

		void ViewCameraComponent(Entity& entity);

		bool m_ShowCreationMenu = false;
		std::string m_CameraName = "New Camera";

	};

}
