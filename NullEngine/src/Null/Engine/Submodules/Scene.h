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
	class Transform
	{

	};
	class sprite
	{

	};

	using EntityID = uint32_t;

	const EntityID MAX_ENTITIES = 5000;

	//class NLE_API IComponentManager;

	class NLE_API Scene
	{
	public:
		Scene(const std::string& name) : m_Name(name)
		{
			m_Entities.reserve(MAX_ENTITIES);
		}
		
		Scene()
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

		const std::string_view GetName() { return m_Name; }

    private:
		class EntityManager
		{
		public:
			EntityManager() {}
			~EntityManager() {}

			EntityID CreateEntity(const std::string& name);

		private:
			EntityID m_NextEntity = 0;
			std::vector<EntityID> m_freeEntities;
			std::unordered_map<EntityID, std::string> m_EntityNames;  // Maps entity IDs to names
		};

		struct Transition 
		{
			Transition(const std::string& f, const std::string& t, const std::string& tr) : from(f), to(t), trigger(tr)
			{}
			std::string from;
			std::string to;
			std::string trigger;
		};

		EntityManager m_EntityManager;

        std::string m_Name;

        std::vector<EntityID> m_Entities;

        std::vector<Transition> m_Transitions;

		//std::unique_ptr<ComponentManager> mComponentManager;
		std::unique_ptr<EntityManager> mEntityManager;
		//std::unique_ptr<SystemManager> mSystemManager;
	};



}