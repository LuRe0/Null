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
#include <string>
#include <memory>


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

    class ArchetypeEditorPannel : public ScenePannel 
    {
    private:
        std::unique_ptr<Scene> m_ArchetypeScene;
        EntityID m_ArchetypeEntity;
        std::string m_CurrentArchetypeName;
        bool m_IsEditing = false;

    public:
        void OpenArchetype(const std::string& archetypeName);

        void OnImGUIRender() override;
        

    private:
        void DrawArchetypeHierarchy();

        void SaveArchetype();

        void CloseArchetype();

    };

}