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
#include <functional>

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
	struct NLE_API BaseComponent;

	class NLE_API IComponentManager
	{
	public:
		virtual ~IComponentManager() {};
		//virtual BaseComponent& Get(EntityID entityID) = 0;
		virtual void Remove(EntityID entityID) = 0;
		virtual void Clear() = 0;
	private:
	};


	class NLE_API IMultiComponentManager : public IComponentManager {
	public:
		virtual ~IMultiComponentManager() {};
		virtual void* GetNamedRaw(EntityID entityID, uint32_t nameID) = 0;
		virtual std::vector<void*> GetAllRaw(EntityID entityID) = 0;
		virtual bool HasNamed(EntityID entityID, uint32_t nameID) const = 0;
		virtual bool HasComponents(EntityID entityID) const  = 0;
		virtual void RemoveNamed(EntityID entityID, uint32_t& nameID) = 0;
	};



	class NLE_API ISingleComponentManager : public IComponentManager {
	public:
		virtual ~ISingleComponentManager() {};
		virtual void* GetRaw(EntityID entityID) = 0;
	};

	template <typename T>
	class NLE_API ComponentManager : public ISingleComponentManager
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

		void* GetRaw(EntityID entityID)  override
		{
			if (!m_EntityToIndexMap.contains(entityID))
			{
				NLE_CORE_ERROR("Entity {0} does not have component", entityID);
				return nullptr;
			}
			return static_cast<void*>(&Get(entityID));
		}

		template<typename... Args>
		void Add(EntityID entityID, Args&&... args)
		{
			m_InternalComponents.emplace_back(std::forward<Args>(args)...);
			size_t newIndex = m_InternalComponents.size() - 1;
			m_EntityToIndexMap[entityID] = newIndex;
			m_IndexToEntityMap[newIndex] = entityID;
		}


		void Remove(EntityID entityID) override
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
			return m_InternalComponents[m_EntityToIndexMap[entityID]];
		}

	private:

		std::vector<T> m_InternalComponents;

		// Map from an entity ID to an array index.
		std::unordered_map<EntityID, size_t> m_EntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, EntityID> m_IndexToEntityMap;

		//// No copying or copy assignment allowed of this class or any derived class
		ComponentManager(ComponentManager const&) =  delete;
		ComponentManager& operator=(ComponentManager const&) = delete;

	};


	template <typename T>
	class NLE_API MultiComponentManager : public IMultiComponentManager
	{
	public:
		using NameID = uint32_t;

		~MultiComponentManager() {}

		MultiComponentManager(int reserveEntities = 64) 
		{
			m_InternalComponents.reserve(reserveEntities);
		}

		bool Empty() const 
		{
			return m_InternalComponents.empty();
		}


		size_t Size() const
		{
			size_t total = 0;
			for (const auto& comps : m_InternalComponents)
				total += comps.size();
			return total;
		}

		void Clear()
		{
			m_InternalComponents.clear();
			m_EntityToIndexMap.clear();
			m_IndexToEntityMap.clear();
			m_NameToIndexMap.clear();
			m_IndexToNameMap.clear();
		}


		void AddEntity(EntityID entityID) {
			if (m_EntityToIndexMap.contains(entityID))
				return; // already added

			size_t newIndex = m_InternalComponents.size();

			// Add mapping entityID <-> newIndex
			m_EntityToIndexMap[entityID] = newIndex;
			m_IndexToEntityMap[newIndex] = entityID;

			// Create a new component vector for this entity, sized to current component count
			m_InternalComponents.emplace_back();
		}

		void AddComponentName(NameID nameID) {
			if (m_NameToIndexMap.contains(nameID))
				return; // already added

			size_t newIndex = m_NameToIndexMap.size();

			// Add mapping nameID <-> newIndex
			m_NameToIndexMap[nameID] = newIndex;
			m_IndexToNameMap[newIndex] = nameID;

			//// Resize all existing entities' component vectors to hold new component slot
			//for (auto& components : m_InternalComponents) {
			//	components.resize(newIndex + 1);
			//}
		}


		template<typename... Args>
		void Add(EntityID entityID, const uint32_t& nameID, Args&&... args)
		{

			AddEntity(entityID);

			size_t index = m_EntityToIndexMap[entityID];

						
			AddComponentName(nameID); // also assumes you have this


			m_InternalComponents[index].emplace_back(std::forward<Args>(args)...);
		}

		void RemoveNamed(EntityID entityID, uint32_t& nameID) override
		{
			if (!m_EntityToIndexMap.contains(entityID)) return;
			if (!m_NameToIndexMap.contains(nameID)) return;

			uint32_t entityIdx = m_EntityToIndexMap[entityID]; // Get the index of the entity in m_InternalComponents

			size_t indexOfRemovedElement = m_NameToIndexMap[nameID]; // get the index of the component name in m_InternalComponents[entityIdx]

			size_t indexOfLastElement = m_InternalComponents[entityIdx].size() - 1; // get the index of the last element in the component vector for this entity


			// swap current with last element
			m_InternalComponents[entityIdx][indexOfRemovedElement] = std::move(m_InternalComponents[entityIdx][indexOfLastElement]);

			m_InternalComponents[entityIdx].pop_back(); // shrink the vector by removing the last element

			// Update map to point to moved spot
			uint32_t compOfLastElement = m_IndexToNameMap[indexOfLastElement];
			m_NameToIndexMap[compOfLastElement] = indexOfRemovedElement;
			m_IndexToNameMap[indexOfRemovedElement] = compOfLastElement;
	
			m_NameToIndexMap.erase(nameID); // remove the entity from the map
			m_IndexToNameMap.erase(indexOfLastElement); // remove the last element from the map



			if(m_InternalComponents[entityIdx].empty())
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
	
		}


		void* GetNamedRaw(EntityID entityID, uint32_t nameID) override
		{
			return static_cast<void*>(&Get(entityID, nameID));
		}


		std::vector<void*> GetAllRaw(EntityID entityID) override
		{
			std::vector<void*> result;

			auto entityIt = m_EntityToIndexMap.find(entityID);
			if (entityIt == m_EntityToIndexMap.end())
				return result;

		
			for (auto& comp : m_InternalComponents[entityIt->second])
				result.emplace_back(static_cast<void*>(&comp));  //


		}

		T& Get(EntityID entityID, const uint32_t& nameID)
		{
			// Check if entity and component name exist
			auto entityIt = m_EntityToIndexMap.find(entityID);
			if (entityIt == m_EntityToIndexMap.end())
				NLE_CORE_THROW("Cannot find Entity {0}", entityID);


			auto nameIt = m_NameToIndexMap.find(nameID);
			if (nameIt == m_NameToIndexMap.end())
				NLE_CORE_THROW("Cannot find Component {0}", nameID);


			size_t entityIndex = entityIt->second;
			size_t nameIndex = nameIt->second;

			// Return pointer to component
			return m_InternalComponents[entityIndex][nameIndex];
		}

		std::vector<std::reference_wrapper<T>> GetAll(EntityID entityID)
		{
			std::vector<std::reference_wrapper<T>> result;

			auto entityIt = m_EntityToIndexMap.find(entityID);
			if (entityIt == m_EntityToIndexMap.end())
				return result;
	
			for (auto& comp : m_InternalComponents[entityIt->second])
				result.emplace_back(comp);  // stores reference safely

			return result;
		}


		bool HasComponents(EntityID entityID) const
		{
			return m_EntityToIndexMap.find(entityID) != m_EntityToIndexMap.end();
		}

		bool HasComponent(EntityID entityID, uint32_t nameID) const
		{
			auto entityIt = m_EntityToIndexMap.find(entityID);
			if (entityIt == m_EntityToIndexMap.end())
				return false;

			auto nameIt = m_NameToIndexMap.find(nameID);
			if (nameIt == m_NameToIndexMap.end())
				return false;


			return true;
		}

		bool HasNamed(EntityID entityID, uint32_t nameID) const override
		{
			return HasComponent(entityID, nameID);
		}



	private:

		void Remove(EntityID entityID) override {};

		// EntityID → (NameID → T)
		// Storage for components: outer vector indexed by entity index, inner vector indexed by name index
		std::vector<std::vector<T>> m_InternalComponents;

		// Map from entityID to its index in m_InternalComponents
		std::unordered_map<EntityID, size_t> m_EntityToIndexMap;

		// Map from entity index back to entityID (optional but useful)
		std::unordered_map<size_t, EntityID> m_IndexToEntityMap;

		// Map from nameID to index in inner vector (name dimension)
		std::unordered_map<NameID, size_t> m_NameToIndexMap;

		// Map from name index back to nameID (optional)
		std::unordered_map<size_t, NameID> m_IndexToNameMap;

		//lets keep an entity to index map for fast access


		MultiComponentManager(MultiComponentManager const&) = delete;
		MultiComponentManager& operator=(MultiComponentManager const&) = delete;
	};

}