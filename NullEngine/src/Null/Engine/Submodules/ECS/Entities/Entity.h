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

		Entity() = default;

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

		template <typename T>
		T& GetFromEntity(EntityID id) 
		{
			return m_Parent->GetComponent<T>(id);
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

		template <typename T>
		void Remove() const
		{
			m_Parent->RemoveComponent<T>(m_ID);
		}

		template <typename T, typename ...TArgs>
		void Add(TArgs&& ...args)
		{
			m_Parent->AddComponent<T>(m_ID, std::forward<TArgs>(args)...);
		}

		template <typename T>
		void Remove()
		{
			m_Parent->RemoveComponent<T>(m_ID);
		}

		template <typename T>
		bool QueryFromEntity(EntityID id) const
		{
			return m_Parent->HasComponent<T>(id);
		}

		BaseComponent& GetComponent(const std::string& componentName);



		bool HasComponent(const std::string& componentName) const;

		void SetIsDestroyed(bool d);

		bool GetIsDestroyed() const;

		void SetName(const std::string& name);

		void SetParentArchetype(const std::string& name);

		void SetArchetype(const std::string& name);

		std::string GetName() const { return m_Name; };
		std::string GetParentName() const { return m_ParentArchetype; };

		EntityID GetID() const { return m_ID; }

		bool operator==(EntityID id) const
		{
			return m_ID == id;
		}


		std::string m_Name;

		std::string m_Archetype;
		
		std::string m_ParentArchetype;

		EntityID m_ID;

		NRegistry* m_Parent;

		bool m_isDestroyed = false;

		friend class SceneHierarchyPannel;
		friend class ComponentInspectorPannel;
	};

	template <typename T>
	auto add_component(Entity& entity, sol::this_state s)
	{
		entity.Add<T>();

		auto& comp = entity.Get<T>();

		return sol::make_reference(s, std::ref(comp));
	}

	template <typename T>
	auto get_component(Entity& entity, sol::this_state s)
	{
		auto& comp = entity.Get<T>();
		return sol::make_reference(s, std::ref(comp));
	}

	template <typename T>
	auto has_component(Entity& entity, sol::this_state s)
	{
		return sol::make_object(s, entity.Has<T>());
	}

	template <typename T>
	auto remove_component(Entity& entity, sol::this_state s)
	{
		entity.Remove<T>();
		return sol::make_object(s, entity.Has<T>());
	}

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