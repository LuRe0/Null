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
#include "../Submodules/ECS/Entities/Entity.h"
#include "NStringIDManager.h"
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
	class Entity;
	class NLE_API NComponentFactory : public ModuleBase<NComponentFactory>
	{
	public:
		void Load() override;

		//! Virtual Init function
		void Init() override {};

		//! Virtual Update function
		void Update(float dt) override {};
		void RuntimeUpdate(float dt) override {};

		void Unload() override {};

		//! Virtual Shutdown function
		void Shutdown() override {};

		static uint32_t GetIdType(const sol::table& comp);

		void RegisterToScripAPI(sol::state& lua) override {};

		//nlohmann::json WriteComponent(BaseComponent* component) const;
		//nlohmann::json DiffComponent(BaseComponent* component, BaseComponent* other) const;

		nlohmann::json WriteComponent(uint32_t componentTypeID, void* component) const;

		nlohmann::json DiffComponent(uint32_t componentTypeID, void* componentA, void* componentB) const;

		void ViewComponent(Entity entity, uint32_t componentId) const;

		std::vector<uint8_t> CreateComponent(const std::string& componentName, const nlohmann::json& componentData) const;

		//std::vector<uint8_t> CreateComponent(const std::string& componentName, const nlohmann::json& componentData, EntityID id) const;

		void AddComponent(const std::string& componentName, void* component, NRegistry* registry, EntityID id) const;

		void AddComponent(uint32_t compID, void* component, NRegistry* registry, EntityID id) const;

		void AddNewComponent(uint32_t compID, NRegistry* registry, EntityID id) const;

		void AddComponentFromBinary(uint32_t compID, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const;

		void AddComponentFromBinary(const std::string& compName, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const;

		void AddNamedComponentFromBinary(const std::string& compName, const uint32_t nameID, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const;

		//BaseComponent& CreateComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

		//void CloneComponent(const std::string& componentName, BaseComponent* component, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

		//void AddComponent(const std::string& componentName, BaseComponent* component, NRegistry* registry, EntityID id) const;

		//void AddComponent(uint32_t compID, BaseComponent* component, NRegistry* registry, EntityID id) const;

		//BaseComponent& CreateUniqueComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const;

		//void CreateComponentsDeferred(const JSON& components, EntityID id);

		//void CreateComponentDeferred(const std::string& name, const JSON& data, EntityID id);

		template <typename T>
		static T* Create()
		{
			return new T();
		}

		template<typename T>
		static void Destroy(void* ptr)
		{
			delete static_cast<T*>(ptr);
		}

		template <typename T, typename ...TArgs>
		static void AddOrUpdate(EntityID entityID, T* newComponent, NRegistry* registry, TArgs&& ...args)
		{
			if (registry->HasComponent<T>(entityID))
			{
				T& component = registry->GetComponent<T>(entityID);

				component = *newComponent;

				NLE_CORE_INFO("Successfully Updated {0}, {1} to entity {2}", Component<T>::TypeName(), Component<T>::GetID(), entityID);

				return;
			}

			registry->AddComponent<T>(entityID, std::forward<TArgs>(args)...);
		}


		template <typename T, typename ...TArgs>
		static void AddOrUpdateNamed(EntityID entityID, uint32_t NameID,  T* newComponent, NRegistry* registry, TArgs&& ...args)
		{
			if (registry->HasNamedComponent<T>(entityID, NameID))
			{
				T& component = registry->GetNamedComponent<T>(entityID, NameID);

				component = *newComponent;

				NLE_CORE_INFO("Successfully Updated {0}, {1} to entity {2}", Component<T>::TypeName(), Component<T>::GetID(), entityID);

				return;
			}

			registry->AddNamedComponent<T>(entityID, NameID, std::forward<TArgs>(args)...);
		}

		template<typename ...Args>
		static auto InvokeSolFunctions(uint32_t componentID, const std::string& func_id, Args&& ...args)
		{
			// Ensure that m_TypeRegistry is accessed correctly
			NLE_CORE_ASSERT(m_TypeRegistry.contains(STRID(func_id)), "Function ID not found");

			NLE_CORE_ASSERT(m_TypeRegistry[STRID(func_id)].contains(componentID), "Component ID not found");

			// Handle case where componentID is not found
			return m_TypeRegistry[STRID(func_id)][componentID](std::forward<Args>(args)...);
		}

		template <typename T>
		void Register(void (*func)(void*, const nlohmann::json&),
			std::function<void(Entity&)> func2,
			std::function<nlohmann::json(const void*)> func3,
			void (*func4)(void*, NRegistry* registry, EntityID id),
			std::function<JSON(const void*, const void*)> func5,
			std::function<void(void*, const uint32_t&)> callback = {})
		{
			AddCreateFunction<T>(func);
			AddViewFunction<T>(func2);
			AddWriteFunction<T>(func3);

			AddComponentID<T>();

			AddSelfAddFunction<T>(func4);
			AddDiffFunction<T>(func5);
			AddLuaFunctions<T>();
			AddBinarySerialization_POD<T>();
			AddNamingFunction<T>(callback);
		}

		std::vector<std::string> GetComponentNames() const
		{
			std::vector<std::string> componentNames;
			for (const auto& pair : m_ComponentNamesToID) {
				componentNames.push_back(STRFROM(pair.first));
			}
			return componentNames;
		}

		uint32_t GetComponentID(const std::string& name) const
		{
			auto it = m_ComponentNamesToID.find(STRID(name));
			if (it != m_ComponentNamesToID.end()) {
				return it->second;
			}
			
			NLE_CORE_THROW("Component: {0} is not registered in facoty", name);
		}



		uint32_t IsMultiple(const uint32_t& compID) const
		{
			auto it = m_ComponentType.find(compID);
			if (it != m_ComponentType.end()) {
				return it->second;
			}

			NLE_CORE_THROW("Component: {0} is not registered in facoty", compID);
		}

	private:
		static std::unordered_map<uint32_t, std::function<void(void*, const nlohmann::json&)>> m_ComponentDeserializer;
		static std::unordered_map<uint32_t, std::function<void(void*, NRegistry*, EntityID)>> m_ComponentAdder;
		static std::unordered_map<uint32_t, std::function<nlohmann::json(const void*)>> m_ComponentSerializer;
		static std::unordered_map<uint32_t, std::function<void(void*, const uint32_t&)>> m_NameIDAssigner;
		// Binary (de)serialization
		static std::unordered_map<uint32_t, std::function<std::vector<uint8_t>(const void*)>> m_ComponentBinarySerializer;
		static std::unordered_map<uint32_t, std::function<void* (const std::vector<uint8_t>, size_t)>> m_ComponentBinaryDeserializer;
		static std::unordered_map<uint32_t, std::function<void* (void)>> m_ComponentCreator;
		static std::unordered_map<uint32_t, std::function<void(void*)>> m_ComponentDestroyer;
		static std::unordered_map<uint32_t, std::function<nlohmann::json(const void*, const void*)>> m_ComponentDiffer;

		static std::unordered_map<uint32_t, size_t> m_ComponentTypeSize;
		static std::unordered_map<uint32_t, uint32_t> m_ComponentNamesToID;
		static std::unordered_map<uint32_t, bool> m_ComponentType;
		static std::unordered_map<uint32_t, std::function<void(Entity&)>> m_ComponentInspector;
		static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::function<sol::object(Entity&, sol::this_state)>>> m_TypeRegistry;

		template <typename T>
		static void AddCreateFunction(void (*func)(void*, const nlohmann::json&))
		{
			m_ComponentCreator.emplace(Component<T>::GetID(), Create<T>);
			m_ComponentDestroyer.emplace(Component<T>::GetID(), Destroy<T>);
			m_ComponentType.emplace(Component<T>::GetID(), Component<T>::AllowMultiple);

			m_ComponentDeserializer[Component<T>::GetID()] = [func](void* component, const nlohmann::json& json) {
				func(component, json);
				};


			m_ComponentTypeSize[Component<T>::GetID()] = sizeof(T);
		}

		template <typename T>
		static void AddNamingFunction(std::function<void(void*, const uint32_t&)> func)
		{
			m_NameIDAssigner[Component<T>::GetID()] = func;
		}


		template <typename T>
		static void AddSelfAddFunction(void (*func)(void*, NRegistry* registry, EntityID id))
		{

			m_ComponentAdder[Component<T>::GetID()] = [func](void* component, NRegistry* registry, EntityID id) {
				func(component, registry, id);
				};
		}

		template <typename T>
		void AddBinarySerializer(std::function<std::vector<uint8_t>(const void*)> func)
		{
			m_ComponentBinarySerializer[Component<T>::GetID()] = func;
		}


		template <typename T>
		void AddBinaryDeserializer(std::function<void* (const std::vector<uint8_t> bin, size_t)> func)
		{
			m_ComponentBinaryDeserializer[Component<T>::GetID()] = func;
		}

		template <typename T>
		void AddBinarySerialization_POD()
		{
			AddBinarySerializer<T>([](const void* comp) -> std::vector<uint8_t> {
				std::vector<uint8_t> blob(sizeof(T));
				std::memcpy(blob.data(), comp, sizeof(T));
				return blob;
				});

			AddBinaryDeserializer<T>([](const std::vector<uint8_t> bin, size_t size) -> void* {
				assert(size == sizeof(T));
				T* instance = new T();
				std::memcpy(instance, bin.data(), size);
				return instance;
				});
		}


		template <typename T>
		void AddDiffFunction(std::function<JSON(const void*, const void*)> func)
		{
			m_ComponentDiffer[Component<T>::GetID()] = func;
		}

		template <typename T>
		static void AddWriteFunction(std::function<nlohmann::json(const void*)> func)
		{
			m_ComponentSerializer.emplace(Component<T>::GetID(), func);
		}

		template <typename T>
		static void AddViewFunction(std::function<void(Entity&)> func2)
		{
			m_ComponentInspector.emplace(Component<T>::GetID(), func2);
		}

		template <typename T>
		static void AddComponentID()
		{
			m_ComponentNamesToID.emplace(STRID(Component<T>::TypeName()), Component<T>::GetID());
		}

		template <typename T>
		static void AddLuaFunctions()
		{
			m_TypeRegistry[STRID("get_component")][Component<T>::GetID()] = [](Entity& entity, sol::this_state s) {
				return get_component<T>(entity, s);
				};

			m_TypeRegistry[STRID("has_component")][Component<T>::GetID()] = [](Entity& entity, sol::this_state s) {
				return has_component<T>(entity, s);
				};

			m_TypeRegistry[STRID("remove_component")][Component<T>::GetID()] = [](Entity& entity, sol::this_state s) {
				return remove_component<T>(entity, s);
				};

			m_TypeRegistry[STRID("add_component")][Component<T>::GetID()] = [](Entity& entity, sol::this_state s) {
				return add_component<T>(entity, s);
				};
		}
	};


	// Definition of static members

}