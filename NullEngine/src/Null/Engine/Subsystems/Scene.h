#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Scene.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


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
	using EntityID = uint32_t;

	class NLE_API Scene
	{
        Scene(const std::string& name);
        ~Scene();

		/// <summary>
		/// This function will be used to read json and load in entities
		/// </summary>
		void Load();

		//!  Init function
		void Init();

		//!  Update function
		void Update(float dt) ;

		//!  function
		void Render();

		void Unload();

		//!  Shutdown function
		void Shutdown();

		const std::string_view& GetNextScene() { return m_NextScene; }

		const std::string_view& GetName() { return m_Name; }

    private:
        std::string m_Name;

		std::string m_NextScene;



        std::vector<EntityID> m_Entity;
	};


	class Layer
	{
	public:
		Layer() =  default;
		~Layer() = default;

	private:

		bool m_Active = 0; // is the scene active
		bool m_MainGame = 0;
	};
}