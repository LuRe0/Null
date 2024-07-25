#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NRegistry.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/ComponentManager.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"
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
	class NLE_API NRegistry : public IModule
	{
	public:
		NRegistry();

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Render();

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;


		EntityID CreateEntity();
		void DestroyEntity(EntityID entityID);
		void AddEntityToSystem(EntityID entityID);

		template <typename T, typename ...TArgs>
		void AddComponent(EntityID entityID, TArgs&& ...args)
		{
			const auto componentID = Component<T>::GetID();

			// Put new entry at end and update the maps
			if (componentID > m_ComponentManagers.size())
			{
				std::unique_ptr<IComponentManager> manager = std::make_unique<ComponentManager<T>>();

				if (componentID - m_ComponentManagers.size() > 1)
				{
					m_ComponentManagers.resize(componentID);
					m_ComponentManagers[componentID-1] = (std::move(manager));
				}
				else
					m_ComponentManagers.push_back(std::move(manager));

			}
			else
			{
				if (!m_ComponentManagers[componentID-1])
				{
					std::unique_ptr<IComponentManager> manager = std::make_unique<ComponentManager<T>>();

					m_ComponentManagers[componentID - 1] = (std::move(manager));
				}
			}

			ComponentManager<T>* newManager = dynamic_cast<ComponentManager<T>*>(m_ComponentManagers[componentID - 1].get());
			if (newManager) {
				// Successfully casted
				if (!HasComponent<T>(entityID))
				{
					newManager->Add(entityID, std::make_unique<T>(std::forward<TArgs>(args)...));
					NLE_CORE_INFO("Succesfully added {0}, {1} to entity {2}", Component<T>::TypeName(), componentID, entityID);
				}
				else
				{
					NLE_CORE_WARN("Attempted to add same {0} {1} to entity {2}", Component<T>::TypeName(), Component<T>::GetID(), entityID);
				}

				m_EntityComponentSignatures[m_EntityToIndexMap[entityID]].set(componentID);

				return;
			}

			NLE_CORE_WARN("Failed to locate appropriate manager");

		}

		template <typename T>
		void RemoveComponent(EntityID entityID)
		{
			const auto componentID = Component<T>::GetID();


			if (HasComponent<T>(entityID))
			{
				ComponentManager<T>* newManager = dynamic_cast<ComponentManager<T>*>(m_ComponentManagers[componentID - 1].get());
				if (newManager)
				{
					newManager->Remove(entityID);
					m_EntityComponentSignatures[m_EntityToIndexMap[entityID]].set(componentID, false);

					NLE_CORE_INFO("Succesfully removed {0}, {1} from entity {2}", Component<T>::TypeName(), componentID, entityID);
					return;
				}
			}
			else
			{
				NLE_CORE_WARN("Attempted to remove {0} {1} which entity {1} does not possess!", Component<T>::TypeName(), Component<T>::GetID(), entityID);
			}



			NLE_CORE_WARN("Failed to locate appropriate manager");
		}

		template <typename T>
		bool HasComponent(EntityID entityID) const
		{
			const auto index = m_EntityToIndexMap.at(entityID);
			const Signature& signature = m_EntityComponentSignatures.at(index);

			return signature.test(Component<T>::GetID());
		}

		template <typename T>
		T& GetComponent(EntityID entityID)
		{
			const auto componentID = Component<T>::GetID();


			if (HasComponent<T>(entityID))
			{
				ComponentManager<T>* newManager = dynamic_cast<ComponentManager<T>*>(m_ComponentManagers[componentID - 1].get());
				if (newManager)
				{
					T& component =newManager->Get(entityID);

					//NLE_CORE_INFO("Succesfully Retrieved {0}, {1} from entity {2}", Component<T>::TypeName(), componentID, entityID);

					return component;
				}
			}
			else
			{
				NLE_CORE_ERROR("Attempted to retrieve {0} {1} which entity {1} does not possess!", Component<T>::TypeName(), Component<T>::GetID(), entityID);
				assert(false);
			}

		}

		template <typename T, typename ...TArgs>
		void AddSystem(TArgs&& ...args)
		{
			if (!m_Systems.contains(std::type_index(typeid(T))))
			{
				//System* system = new T();
				std::unique_ptr<ISystem> system = std::make_unique<T>(std::forward<TArgs>(args)...);

				system.get()->SetParent(this);

				m_Systems.emplace(std::make_pair(std::type_index(typeid(T)), std::move(system)));

				NLE_CORE_INFO("Succesfully Added {0} to manager", System<T>::TypeName());

				return;
			}

			NLE_CORE_WARN("{0} already registered!", System<T>::TypeName());

		}

		template <typename T>
		void RemoveSystem()
		{
			const auto key = std::type_index(typeid(T));
			if (m_Systems.contains(key))
			{
				m_Systems.erase(key);

				NLE_CORE_INFO("Succesfully Removed {0}", System<T>::TypeName());

				return;
			}

			NLE_CORE_WARN("Attempted to remove a {0} which is not registered!", System<T>::TypeName());
		}

		template <typename T>
		bool HasSystem() const
		{
			const auto key = std::type_index(typeid(T));

			return m_Systems.contains(key);
		}

		template <typename T>
		T& GetSystem() const
		{
			const auto key = std::type_index(typeid(T));
			if (m_Systems.contains(key))
			{
				NLE_CORE_INFO("Succesfully Retrieved {0}", System<T>::TypeName());
				return static_cast<T&>(*(m_Systems.at(key)));
			}

			NLE_CORE_ERROR("Attempted to retrieve a {0} which is not registered!", System<T>::TypeName());
			assert(false);
		}

		template <typename T>
		void AddCreateFunction(std::function<void() > function)
		{
			m_Createfunctions.emplace(System<T>::TypeName(), function);
		}

		const Signature& EntitySignature(EntityID id);

		static const std::string Name() { return "Registry"; }


	private:
		/// <summary>
		/// vector of component managers
		/// vector in dex = component type id
		/// </summary>
		std::vector<std::unique_ptr<IComponentManager>> m_ComponentManagers;

		// Map from an entity ID to an array index.
		std::unordered_map<EntityID, size_t> m_EntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, EntityID> m_IndexToEntityMap;

		/// <summary>
		/// vector of entity component signature to keep track of which systems should be aware of them
		/// vector index = entity id
		/// </summary>
		std::vector<Signature> m_EntityComponentSignatures;

		/// <summary>
		/// 
		/// </summary>
		std::unordered_map<std::type_index, std::unique_ptr<ISystem>> m_Systems;

		size_t m_NumEntities;

		std::queue<EntityID> m_RecycledEntity;

		NRegistry(NRegistry const&);
		NRegistry& operator=(NRegistry const&);

		std::unordered_map<std::string, std::function<void()>> m_Createfunctions;
	};

}