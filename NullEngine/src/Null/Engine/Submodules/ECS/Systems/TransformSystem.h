#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	TransformSystem.h
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
	class TransformComponent;

	class NLE_API TransformSystem : public ISystem
	{
	public:

		TransformSystem();

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

		TransformComponent* Transform(Entity& entity);

		void SetTranslation( Entity& entity, glm::vec3 position);
		void SetRotation( Entity& entity, glm::vec3 rotation);
		void SetScale( Entity& entity, glm::vec3 scale);

	private:
		//void OnWindowResize(const WindowResizeEvent& e);
		static void CreateTransformComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		static JSON WriteTransformComponent(BaseComponent* component);

		void ViewTransformComponent(Entity& entity);

	};

}
