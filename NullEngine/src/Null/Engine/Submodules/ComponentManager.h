#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ComponentManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Tools/Trace.h"


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
	class NLE_API BaseComponent;

	class NLE_API IComponentManager
	{
	public:
		virtual ~IComponentManager() {};
		virtual BaseComponent& Get(EntityID entityID) = 0;
		virtual void Remove(EntityID entityID) = 0;
		virtual void Clear() = 0;
	private:
	};

	template <typename T>
	class NLE_API ComponentManager : public IComponentManager
	{
	public:
		~ComponentManager() {}

		ComponentManager(int size = 64)
		{
			m_InternalComponents.reserve(size);
		}

		bool Empty() const
		{
			return m_InternalComponents.empty();
		}

		size_t Size() const
		{
			return m_InternalComponents.size();
		}

		void Resize(int s)
		{
			m_InternalComponents.resize(s);
		}

		void Clear() override
		{
			m_InternalComponents.clear();
			m_EntityToIndexMap.clear();
			m_IndexToEntityMap.clear();
		}

		void Add(EntityID entityID, std::unique_ptr<T>&& component)
		{
			// Put new entry at end and update the maps
			m_InternalComponents.emplace_back(std::move(component));
			size_t newIndex = m_InternalComponents.size() - 1;
			m_EntityToIndexMap[entityID] = newIndex;
			m_IndexToEntityMap[newIndex] = entityID;
		}

		void Remove(EntityID entityID)
		{
			size_t indexOfRemovedEntity = m_EntityToIndexMap[entityID];
			size_t indexOfLastElement = m_InternalComponents.size() - 1;
			m_InternalComponents[indexOfRemovedEntity] = std::move(m_InternalComponents[indexOfLastElement]);
			m_InternalComponents.pop_back(); // deleting component from array might change that later

			// Update map to point to moved spot
			EntityID entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
			m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndexMap.erase(entityID);
			m_IndexToEntityMap.erase(indexOfLastElement);
		}

		T& Get(EntityID entityID)
		{
			return static_cast<T&>(*(m_InternalComponents[m_EntityToIndexMap[entityID]]));
		}
	private:
		std::vector<std::unique_ptr<T>> m_InternalComponents;

		// Map from an entity ID to an array index.
		std::unordered_map<EntityID, size_t> m_EntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, EntityID> m_IndexToEntityMap;

		//// No copying or copy assignment allowed of this class or any derived class
		ComponentManager(ComponentManager const&) =  delete;
		ComponentManager& operator=(ComponentManager const&) = delete;

	};

}