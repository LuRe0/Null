#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	SceneHierarchyPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

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
	class Scene;

	class SceneHierarchyPannel : public Pannel
	{
	public:
		SceneHierarchyPannel() = default;
		~SceneHierarchyPannel() = default;

		void OnImGUIRender();
	private:
		void DrawEntityNode(Entity& entity);
		SceneHierarchyPannel(SceneHierarchyPannel const&) = delete;
		SceneHierarchyPannel& operator=(SceneHierarchyPannel const&) = delete;

		void ParentEntities(Entity& entity);
		void SeparateEntities(Entity& entity);
	};

}