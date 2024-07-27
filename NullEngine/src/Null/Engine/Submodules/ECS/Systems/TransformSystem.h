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




	private:
		//void OnWindowResize(const WindowResizeEvent& e);
		static void CreateTransformComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		const glm::vec2 MetersToPixels(float x, float y);
		const glm::vec2 PixelsToMeters(float x, float y);



		void ViewTransformComponent(Entity& entity);

	};

}
