#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NComponentFactory.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"

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
	class NLE_API NComponentFactory : public IModule
	{
	public:

		void Load() override;

		//! Virtual Init function
		void Init() override {};

		//! Virtual Update function
		void Update(float dt) override {};

		void Unload() override {};
		//! Virtual Shutdown function
		void Shutdown() override {};

	
		BaseComponent& CreateComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
		{
			BaseComponent* component = m_ComponentCreator.at(componentName)();

			auto it = m_componentReader.find(componentName);
			if (it != m_componentReader.end()) {
				it->second(component, componentData, registry, id);
			}

			return *component;
		}

		void CloneComponent(const std::string& componentName, BaseComponent* component, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
		{
			auto it = m_componentReader.find(componentName);
			if (it != m_componentReader.end()) {
				it->second(component, componentData, registry, id);
			}
		}


		void CreateUniqueComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
		{
			BaseComponent* component = m_ComponentCreator.at(componentName)();

			auto it = m_componentReader.find(componentName);
			if (it != m_componentReader.end()) {
				it->second(component, componentData, registry, id);
			}

			delete component;
		}

		template <typename T>
		static BaseComponent* Create() 
		{
			return new T();
		}


		template <typename T, typename ...TArgs>
		static void AddOrUpdate(EntityID entityID, T* newComponent, NRegistry* registry, TArgs&& ...args)
		{
			if (registry->HasComponent<T>(entityID))
			{
				T& component = registry->GetComponent<T>(entityID);

				component = *newComponent;

				NLE_CORE_INFO("Succesfully Updated {0}, {1} to entity {2}", Component<T>::TypeName(), Component<T>::GetID(), entityID);

				return;
			}

			registry->AddComponent<T>(entityID, std::forward<TArgs>(args)...);
		}


		//void AddComponentCreateFunction(std::string_view component, std::function < Component& (Entity&)> function);


		template <typename T>
		void Register(void (*func)(void*, const nlohmann::json&, NRegistry* registry, EntityID id))
		{
			AddCreateFunction<T>(func);
		}

	private:
		std::unordered_map<std::string, std::function<void(void*, const nlohmann::json&, NRegistry*, EntityID)>> m_componentReader;
		std::unordered_map<std::string, std::function<BaseComponent* ()>> m_ComponentCreator;

		

		template <typename T>
		void AddCreateFunction(void (*func)(void*, const nlohmann::json&, NRegistry* registry, EntityID id))
		{
			m_ComponentCreator.emplace(Component<T>::TypeName(), Create<T>);

			m_componentReader[Component<T>::TypeName()] = [func](void* component, const nlohmann::json& json, NRegistry* registry, EntityID id) {
				func(component, json, registry, id);
				};
		}

	};

}