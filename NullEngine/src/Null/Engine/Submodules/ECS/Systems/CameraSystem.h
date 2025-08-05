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
	//class CameraComponent;

	class NLE_API CameraSystem : public ISystem<CameraSystem>
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
		void RenderImGui() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;


		void RegisterToScripAPI(sol::state& lua) override;


	private:
		bool OnRuntimeStart(const EngineRunStateEvent& e);
		static void CreateCameraComponent(void* component, const nlohmann::json& json);

		static void AddCameraComponent(void* component, NRegistry* registry, EntityID id);

		static JSON WriteCameraComponent(const void* component);

		static JSON DiffCameraComponent(const void* base, const void* modified);

		void AddShake(const CameraShake& shake);

		void UpdateCameraShake(float dt);

		void ResetCameraStates();

		void ViewCameraComponent(Entity& entity);

		bool m_ShowCreationMenu = false;
		bool m_Preview = false;
		std::string m_CameraName = "New Camera";
		unsigned int m_CameraType = 0;
		size_t m_MainCameraCount = 0;



		std::vector<CameraShake> m_ActiveShakes;
	};

}
