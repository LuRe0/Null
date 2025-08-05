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
#include <vector>
#include <functional>

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
	struct NLE_API DestroyedComponent;

	class NLE_API Entity
	{
	public:
		Entity(EntityID id, NRegistry* parent);

		Entity();

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

		T& Get(uint32_t id)
		{
			return m_Parent->GetNamedComponent<T>(m_ID, id);
		}

	/*	template <typename T>
		T& GetFromEntity(EntityID id) 
		{
			return m_Parent->GetComponent<T>(id);
		}*/

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

		template <typename T>
		void Remove(uint32_t id) const
		{
			m_Parent->RemoveNamedComponent<T>(m_ID, id);
		}

		template <typename T>
		bool QueryFromEntity(EntityID id) const
		{
			return m_Parent->HasComponent<T>(id);
		}

		template <typename T>
		T& GetFromEntity(EntityID id)
		{
			return m_Parent->GetComponent<T>(id);
		}


		template <typename T, typename ...TArgs>
		void Add(TArgs&& ...args)
		{
			m_Parent->AddComponent<T>(m_ID, std::forward<TArgs>(args)...);
		}

		template <typename T>
		std::vector<std::reference_wrapper<T>> GetAll()
		{
			return m_Parent->GetNamedComponents<T>(m_ID);
		}

		operator bool() const { return IsValid(); }


		template <typename T>
		T* TryGet()
		{
			return Has<T>() ? &Get<T>() : nullptr;
		}


		bool HasComponent(const std::string& componentName) const;


		EntityID GetID() const { return m_ID; }

		bool IsValid() const
		{
			return m_ID > 0 && !Has<DestroyedComponent>();
		}

		bool operator==(EntityID id) const
		{
			return m_ID == id;
		}


		EntityID m_ID = -1;

		NRegistry* m_Parent = nullptr;

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