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
#include "Null/Engine/Modules/NRegistry.h"

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
	class NLE_API NRegistry;

	class NLE_API Entity
	{
	public:
		Entity(EntityID id, NRegistry* parent);


		/// <summary>
		/// Gets A component attached to the entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template <typename T>
		T& Get()
		{
			return m_Parent->GetComponent<T>(m_ID);
		}

		/// <summary>
		/// Checks if entity posseses component
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template <typename T>
		bool Has() const
		{
			return m_Parent->HasComponent<T>(m_ID);
		}


		void SetIsDestroyed(bool d);

		bool GetIsDestroyed() const;

		void SetName(const std::string& name);

		std::string GetName() const { return m_Name; };

		EntityID GetID() const { return m_ID; }

		bool operator==(EntityID id) const
		{
			return m_ID == id;
		}

	private:
		std::string m_Name;

		EntityID m_ID;

		NRegistry* m_Parent;

		bool m_isDestroyed = false;
	};

	//template<typename T>
	//inline T& Entity::Get()
	//{
	//	// TODO: insert return statement here
	//	return m_Parent->GetComponent<T>(m_ID);
	//}

	//template<typename T>
	//inline bool Entity::Has() const
	//{
	//	return m_Parent->HasComponent<T>(m_ID);
	//}

}