#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Entity.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "nlohmann/json.hpp"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

using JSON = nlohmann::json;

using EntityID = uint32_t;

//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
	class NLE_API Entity
	{
	public:
		Entity(EntityID id);


		/// <summary>
		/// Gets A component attached to the entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template <typename T>
		T& Get();

		/// <summary>
		/// Attach a component to an entity
		/// </summary>
		void Add();

		/// <summary>
		/// reads an entity's information from a file
		/// </summary>
		void Read(const JSON& entityData, NRegistry* registry);

		/// <summary>
		/// Calls components load functions
		/// </summary>
		void Load();

		/// <summary>
		/// calls attached components init functions
		/// </summary>
		void Init();

		/// <summary>
		/// calls attached components shutdown function.
		/// </summary>
		void Shutdown();

		void SetName(const std::string& name);

		const std::string GetName() { return m_Name; };

		EntityID GetID() const { return m_ID; }

	private:
		std::string m_Name;

		EntityID m_ID;
	};

}