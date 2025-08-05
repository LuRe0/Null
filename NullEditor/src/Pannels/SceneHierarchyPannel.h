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
#include "../Pannels/Base/Pannel.h"


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
	class Entity;

	class SceneHierarchyPannel : public ScenePannel
	{
	public:
		SceneHierarchyPannel() = default;
		~SceneHierarchyPannel() = default;

		void OnImGUIRender();
	private:
		void DrawEntityNode(Entity& entity);
		SceneHierarchyPannel(SceneHierarchyPannel const&) = delete;
		SceneHierarchyPannel& operator=(SceneHierarchyPannel const&) = delete;

		void ParentEntities(Entity& entity,Entity& parentEntity);
		void SeparateEntities(Entity& entity);


		void SelectedEntityMenuItems(Entity& entity);


		//ImGuiTextFilter m_EntityFilter;

	};

}