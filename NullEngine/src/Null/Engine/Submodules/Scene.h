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
#include "stdafx.h"
#include "Null/Core.h"
#include "nlohmann/json.hpp"
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"


using JSON = nlohmann::json;


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

	const EntityID MAX_ENTITIES = 5000;



	//class NLE_API IComponentManager;

	class NLE_API Scene
	{
	public:
		Scene(const std::string& name) : m_Name(name), m_Transitions(std::vector<Transition>()), m_Entities(std::vector<Entity>())
		{
			m_Entities.reserve(MAX_ENTITIES);
		}

		~Scene() {}

		/// <summary>
		/// This function will be used to read json and load in entities
		/// </summary>
		void Load(const std::string& name, const JSON& jsonData);

		//!  Init function
		void Init();

		//!  Update function
		void Update(float dt) ;

		//!  function
		void Render();

		void Unload();

		//!  Shutdown function
		void Shutdown();

		const std::vector<Entity>& GetManagedEntities() { return m_Entities; }

		const std::string_view GetName() { return m_Name; }

    private:
		Scene(Scene const&) = delete;
		Scene& operator=(Scene const&) = delete;

		struct Transition 
		{
			Transition(const std::string& f, const std::string& t, const std::string& tr) : from(f), to(t), trigger(tr)
			{}
			std::string from;
			std::string to;
			std::string trigger;
		};

		//EntityManager m_EntityManager;

        std::string m_Name;

        std::vector<Entity> m_Entities;

        std::vector<Transition> m_Transitions;

		//std::unique_ptr<ComponentManager> mComponentManager;
		//std::unique_ptr<EntityManager> m_EntityManager;
		//std::unique_ptr<SystemManager> mSystemManager;
	};



}