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
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"

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

		void RegisterToScripAPI(sol::state& lua) override {};

		nlohmann::json WriteComponent(BaseComponent* component) const;

		void ViewComponent(Entity entity, uint32_t componentId) const;
	
		BaseComponent& CreateComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

		void CloneComponent(const std::string& componentName, BaseComponent* component, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

		void CreateUniqueComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

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
		void Register(void (*func)(void*, const nlohmann::json&, NRegistry* registry, EntityID id),
									std::function<void(Entity&)> func2,
									std::function<nlohmann::json(BaseComponent*)> func3)
		{
			AddCreateFunction<T>(func);
			AddViewFunction<T>(func2);
			AddWriteFunction<T>(func3);
			AddComponentID<T>();
		}

		std::vector<std::string> GetComponentNames() const 
		{
			std::vector<std::string> componentNames;
			for (const auto& pair : m_ComponentCreator) {
				componentNames.push_back(pair.first);
			}
			return componentNames;
		}


		uint32_t GetComponentID(const std::string& name) const
		{
			auto it = m_ComponentNamesToID.find(name);
			if (it != m_ComponentNamesToID.end()) {
				return it->second;
			}
		}

	private:
		std::unordered_map<std::string, std::function<void(void*, const nlohmann::json&, NRegistry*, EntityID)>> m_componentReader;
		std::unordered_map<std::string, std::function<nlohmann::json(BaseComponent*)>> m_componentWriter;
		std::unordered_map<std::string, std::function<BaseComponent* ()>> m_ComponentCreator;
		std::unordered_map<std::string, uint32_t> m_ComponentNamesToID;
		std::unordered_map<size_t, std::function<void (Entity&)>> m_ComponentInspector;

		template <typename T>
		void AddCreateFunction(void (*func)(void*, const nlohmann::json&, NRegistry* registry, EntityID id))
		{
			m_ComponentCreator.emplace(Component<T>::TypeName(), Create<T>);

			m_componentReader[Component<T>::TypeName()] = [func](void* component, const nlohmann::json& json, NRegistry* registry, EntityID id) {
				func(component, json, registry, id);
				};
		}


		template <typename T>
		void AddWriteFunction(std::function<nlohmann::json(BaseComponent*)> func)
		{
			m_componentWriter.emplace(Component<T>::TypeName(), func);
		}

		template <typename T>
		void AddViewFunction(std::function<void(Entity&)> func2)
		{
			m_ComponentInspector.emplace(Component<T>::GetID(), func2);
		}

		template <typename T>
		void AddComponentID()
		{
			m_ComponentNamesToID.emplace(Component<T>::TypeName(), Component<T>::GetID());
		}
	};

}