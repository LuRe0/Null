#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	System.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"
#include "../../../Modules/NRegistry.h"
#include "../../../Modules/NSceneManager.h"
#include "../../../Modules/NEventManager.h"
#include "../../Events/CommonEvents.h"
#include "../Helpers/ComponentSignature.h"
#include "../../Scene.h"
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
	
	class NLE_API NRegistry;
	class NLE_API NEventManager;

	template<typename Derived>
	class NLE_API ISystem : public ModuleBase<Derived>
	{
	public:
		virtual ~ISystem() {};

		virtual void Load()  override = 0;
		//! Virtual Init function
		virtual void Init() override;
		//! Virtual Update function
		virtual void Update(float dt) override = 0;
		virtual void RuntimeUpdate(float dt) override {}

		virtual void Render() override = 0;

		virtual void RenderImGui() override;

		virtual void Unload()override = 0;
		//! Virtual Shutdown function
		virtual void Shutdown() override = 0;


		virtual bool HasRenderImGui() const override { return !m_Entities.empty(); }


		/// <summary>
		/// Adds entity to system
		/// </summary>
		/// <param name="entity"></param>
		void Add(EntityID entity);


		/// <summary>
		/// removes an enitity from the system
		/// </summary>
		void Remove(EntityID entity);

		/// <summary>
		/// get all entities relevent to a system
		/// </summary>
		/// <returns></returns>
		const std::vector<EntityID>& GetSystemEntities() const;


		const SignatureBits& GetComponentSignature() const;

		/// <summary>
		/// Defines which components an entity must possess to be modified by system
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T> 
		void Require()
		{
			const auto id = Component<T>::GetID();
			m_ComponentSignatures.set(id);
		}

		//void SetParent(NRegistry* registry)
		//{
		//	m_Parent = registry;
		//}
	private:
		/// <summary>
		/// the componets an entity must possess in order to be modified by the system
		/// </summary>
		SignatureBits m_ComponentSignatures;

		/// <summary>
		/// entities that are modifiable by the system
		/// </summary>

		void CheckEntity(EntityID entityID, NRegistry* registry);
		void UpdateEntityList(EntityID entityID, NRegistry* registry);


		bool OnEntityCreate(const EntityModifiedEvent& e);
		bool OnEntityDestroyed(const EntityDestroyedEvent& e);
		bool OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		bool OnEntityComponentAdded(const EntityAddComponentEvent& e);
		bool OnSceneSwitch(const SceneSwitchEvent& e);


	protected:
		std::vector<EntityID> m_Entities;
		//NRegistry* m_Parent;
	};

	template<typename Derived>
	void ISystem<Derived>::Load()
	{
	}

	template<typename Derived>
	void ISystem<Derived>::Init()
	{
		NRegistry* registry = NRegistry::Instance();
		NSceneManager* scMan = NSceneManager::Instance();
		Scene* currScene = scMan->GetCurrentScene();

		auto& entities = currScene->GetManagedEntities();

		for (auto& entity : entities)
		{
			CheckEntity(entity.GetID(), registry);
		}

		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(EntityCreatedEvent, &ISystem<Derived>::OnEntityCreate, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &ISystem<Derived>::OnEntityCreate, eventManager, EventPriority::Medium);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &ISystem<Derived>::OnEntityComponentRemoved, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityDestroyedEvent, &ISystem<Derived>::OnEntityDestroyed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ISystem<Derived>::OnSceneSwitch, eventManager, EventPriority::Medium);
	}


	template<typename Derived>
	void ISystem<Derived>::Update(float dt)
	{
	}


	template<typename Derived>
	void ISystem<Derived>::RenderImGui()
	{
		ImGui::Text("Registered Entities: %d", m_Entities.size());
	}

	template<typename Derived>
	void ISystem<Derived>::Unload()
	{
	}

	template<typename Derived>
	void ISystem<Derived>::Shutdown()
	{
	}

	template<typename Derived>
	void ISystem<Derived>::Add(EntityID entity)
	{
		if (std::find(m_Entities.begin(), m_Entities.end(), entity) == m_Entities.end())
			m_Entities.push_back(entity);
	}

	template<typename Derived>
	void ISystem<Derived>::Remove(EntityID entity)
	{
		auto ent = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (ent != m_Entities.end())
		{
			NLE_CORE_INFO("Entity found in list: {0}", *ent);

			const int index = static_cast<int>(ent - m_Entities.begin());
			std::swap(m_Entities[index], m_Entities.back());

			m_Entities.pop_back();

			NLE_CORE_INFO("Entity Successfully Removed.");
		}
		else
			NLE_CORE_WARN("Entity not found in list!");
	}

	template<typename Derived>
	const std::vector<EntityID>& ISystem<Derived>::GetSystemEntities() const
	{
		// TODO: insert return statement here
		return m_Entities;
	}

	template<typename Derived>
	const SignatureBits& ISystem<Derived>::GetComponentSignature() const
	{
		// TODO: insert return statement here
		return m_ComponentSignatures;
	}

	template<typename Derived>
	void ISystem<Derived>::CheckEntity(EntityID entityID, NRegistry* registry)
	{
		const auto& entityComponentSignatures = registry->EntitySignature(entityID);

		const auto& systemComponentSignatures = GetComponentSignature();

		bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

		if (match)
			Add(entityID);
	}

	template<typename Derived>
	void ISystem<Derived>::UpdateEntityList(EntityID entityID, NRegistry* registry)
	{
		const auto& entityComponentSignatures = registry->EntitySignature(entityID);

		const auto& systemComponentSignatures = GetComponentSignature();

		bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

		if (!match)
		{
			Remove(entityID);
		}
		else
		{
			Add(entityID);
		}
	}

	template<typename Derived>
	bool ISystem<Derived>::OnEntityCreate(const EntityModifiedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		CheckEntity(e.GetID(), registry);
		return true;
	}

	template<typename Derived>
	bool ISystem<Derived>::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		UpdateEntityList(e.GetID(), registry);
		return true;
	}


	template<typename Derived>
	bool ISystem<Derived>::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		UpdateEntityList(e.GetID(), registry);
		return true;
	}


	template<typename Derived>
	bool ISystem<Derived>::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		m_Entities.clear();
		return true;
	}

	template<typename Derived>
	bool ISystem<Derived>::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		Remove(e.GetID());
		return true;
	}
	
	template <typename T>
	class System
	{
	public:

		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}

	private:

	

	};



}
