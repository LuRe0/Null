
//------------------------------------------------------------------------------
//
// File Name:	NNComponentFactory.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NComponentFactory.h"
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"
#include "../Submodules/ECS/Entities/Entity.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	NLE_API std::unordered_map<uint32_t, std::function<void(void*, const nlohmann::json&)>> NComponentFactory::m_ComponentDeserializer;
	NLE_API std::unordered_map<uint32_t, std::function<void(void*, const uint32_t&)>> NComponentFactory::m_NameIDAssigner;
	NLE_API std::unordered_map<uint32_t, std::function<void(void*, NRegistry*, EntityID)>> NComponentFactory::m_ComponentAdder;
	NLE_API std::unordered_map<uint32_t, std::function<nlohmann::json(const void*)>> NComponentFactory::m_ComponentSerializer;
	NLE_API std::unordered_map<uint32_t, size_t> NComponentFactory::m_ComponentTypeSize;
	NLE_API std::unordered_map<uint32_t, std::function<std::vector<uint8_t>(const void*)>> NComponentFactory::m_ComponentBinarySerializer;
	NLE_API std::unordered_map<uint32_t, std::function<void* (const std::vector<uint8_t>, size_t)>> NComponentFactory::m_ComponentBinaryDeserializer;
	NLE_API std::unordered_map<uint32_t, std::function<void* ()>> NComponentFactory::m_ComponentCreator;
	NLE_API std::unordered_map<uint32_t, std::function<void (void*)>> NComponentFactory::m_ComponentDestroyer;
	NLE_API std::unordered_map<uint32_t, uint32_t> NComponentFactory::m_ComponentNamesToID;
	NLE_API std::unordered_map<uint32_t, bool> NComponentFactory::m_ComponentType;

	NLE_API std::unordered_map<uint32_t, std::function<void(Entity&)>> NComponentFactory::m_ComponentInspector;
	NLE_API std::unordered_map<uint32_t, std::function<JSON(const void*, const void*)>> NComponentFactory::m_ComponentDiffer;

	NLE_API std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::function<sol::object(Entity&, sol::this_state)>>> NComponentFactory::m_TypeRegistry;

	void NComponentFactory::Load()
	{
		//AddCreateFunction<TransformComponent>(CreateTransformComponent);
		//AddCreateFunction<SpriteComponent>(CreateSpriteComponent);
		//AddCreateFunction<Rigidbody2DComponent>(CreateRigidbody2DComponent);
		//AddCreateFunction<BoxCollider2DComponent>(CreateBoxCollider2DComponent);
		//AddCreateFunction<AnimationComponent>(CreateAnimationComponent);
	}



	//auto& NComponentFactory::Resolve(BaseComponent& comp)
	//{
	//	if (comp.ID() == Component<TransformComponent>::GetID())
	//	{
	//		// Perform a safe downcast to the derived component type
	//		return static_cast<TransformComponent&>(comp);
	//	}
	//	else if (comp.ID() == Component<SpriteComponent>::GetID())
	//	{
	//		return static_cast<SpriteComponent&>(comp);
	//	}

	uint32_t NComponentFactory::GetIdType(const sol::table& comp)
	{
		NLE_CORE_ASSERT(comp.valid(), "Failed to get the type id");

	
		const auto func = comp["type_id"].get<sol::function>();

		NLE_CORE_ASSERT(func.valid(), "[type_id()] - function not exposed to lua");

		return func().get<uint32_t>();
	}

	//	// Handle other component types or throw an exception
	//	NLE_CORE_WARN("Component type not recognized or supported.");
	//}
	nlohmann::json NComponentFactory::WriteComponent(uint32_t componentTypeID, void* component) const
	{
		auto it = m_ComponentSerializer.find(componentTypeID);
		if (it != m_ComponentSerializer.end())
		{
			return it->second(component);
		}

		NLE_CORE_WARN("No Write function found for component ID {0}", componentTypeID);
		return nlohmann::json();
	}

	nlohmann::json NComponentFactory::DiffComponent(uint32_t componentTypeID, void* componentA, void* componentB) const
	{
		auto it = m_ComponentDiffer.find(componentTypeID);
		if (it != m_ComponentDiffer.end())
		{
			return it->second(componentA, componentB);
		}

		NLE_CORE_WARN("No Diff function found for component ID {0}", componentTypeID);
		return nlohmann::json();
	}


	void NComponentFactory::ViewComponent(Entity entity, uint32_t componentId) const
	{
		auto it = m_ComponentInspector.find(componentId);
		if (it != m_ComponentInspector.end()) {
			it->second(entity);
		}
		//else
		//	NLE_CORE_WARN("No Inspector function found for {0}", componentId);
	}

	
	std::vector<uint8_t> NComponentFactory::CreateComponent(const std::string& componentName, const nlohmann::json& componentData) const
	{
		uint32_t typeID = m_ComponentNamesToID[STRID(componentName)];

		auto creatorIt = m_ComponentCreator.find(typeID);
		if (creatorIt == m_ComponentCreator.end()) {
			NLE_CORE_WARN("No creator found for component: {0}", componentName);
			return {};
		}

		// Allocate raw memory
		void* component = creatorIt->second(); // Construct default-initialized POD component

		// Deserialize into memory
		auto deserializeIt = m_ComponentDeserializer.find(typeID);
		if (deserializeIt != m_ComponentDeserializer.end()) {
			deserializeIt->second(component, componentData);
		}
		else {
			NLE_CORE_WARN("No deserializer found for component: {0}", componentName);
		}

		// Get type size (you'll need to register this info per component type)
		size_t compSize = m_ComponentTypeSize.at(typeID);

		// Copy into binary blob
		//std::vector<uint8_t> blob(compSize);
		//std::memcpy(blob.data(), component, compSize);


		auto binDeserializeIt = m_ComponentBinarySerializer.find(typeID);
		if (binDeserializeIt != m_ComponentBinarySerializer.end()) {
			std::vector<uint8_t> bin = binDeserializeIt->second(component);

			m_ComponentDestroyer[typeID](component);

			return bin;
		}
		else {
			NLE_CORE_THROW("No deserializer found for component: {0}", componentName);
		}
	}


	//void NComponentFactory::CloneComponent(const std::string& componentName, BaseComponent* component, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
	//{
	//	auto it = m_componentDeserializer.find(STRID(componentName));
	//	if (it != m_componentDeserializer.end()) {
	//		it->second(component, componentData, id);
	//	}
	//	else
	//	{
	//		NLE_CORE_WARN("No Read function found for {0}", component->Name());
	//	}
	//}

	void NComponentFactory::AddComponent(const std::string& componentName, void* component, NRegistry* registry, EntityID id) const
	{
		auto it = m_ComponentAdder.find(m_ComponentNamesToID[STRID(componentName)]);
		if (it != m_ComponentAdder.end()) {
			it->second(component, registry, id);
		}
		else
		{
			NLE_CORE_WARN("No Read function found for {0}", componentName);
		}
	}


	void NComponentFactory::AddComponent(uint32_t compID, void* component, NRegistry* registry, EntityID id) const
	{
		auto it = m_ComponentAdder.find(compID);
		if (it != m_ComponentAdder.end()) {
			it->second(component, registry, id);
		}
		else
		{
			NLE_CORE_WARN("No Add function found for component ID {}", compID);
		}
	}


	void NComponentFactory::AddComponentFromBinary(uint32_t compID, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const
	{
		auto adderIt = m_ComponentAdder.find(compID);
		auto sizeIt = m_ComponentTypeSize.find(compID);

		if (adderIt == m_ComponentAdder.end())
		{
			NLE_CORE_WARN("No component adder found for component ID {}", STRFROM(compID));
			return;
		}

		if (sizeIt == m_ComponentTypeSize.end())
		{
			NLE_CORE_WARN("No type size registered for component ID {}", STRFROM(compID));
			return;
		}

		size_t expectedSize = sizeIt->second;

		if (blob.size() != expectedSize)
		{
			NLE_CORE_WARN("Blob size mismatch for component ID {}: expected {}, got {}", STRFROM(compID), expectedSize, blob.size());
			return;
		}


		auto binDeserializeIt = m_ComponentBinaryDeserializer.find(compID);
		if (binDeserializeIt != m_ComponentBinaryDeserializer.end()) 
		{
			void* component = binDeserializeIt->second(blob, expectedSize);


			adderIt->second(component, registry, id);

			m_ComponentDestroyer[compID](component);


		}
		else 
		{
			NLE_CORE_THROW("No deserializer found for component: {0}", STRFROM(compID));
		}
	}

	void NComponentFactory::AddComponentFromBinary(const std::string& compName, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const
	{
		AddComponentFromBinary(m_ComponentNamesToID[STRID(compName)], blob, registry, id);
	}

	void NComponentFactory::AddNamedComponentFromBinary(const std::string& compName, const uint32_t nameID, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const
	{
		uint32_t compID = m_ComponentNamesToID[STRID(compName)];


		auto adderIt = m_ComponentAdder.find(compID);
		auto sizeIt = m_ComponentTypeSize.find(compID);

		if (adderIt == m_ComponentAdder.end())
		{
			NLE_CORE_WARN("No component adder found for component ID {}", STRFROM(compID));
			return;
		}

		if (sizeIt == m_ComponentTypeSize.end())
		{
			NLE_CORE_WARN("No type size registered for component ID {}", STRFROM(compID));
			return;
		}

		size_t expectedSize = sizeIt->second;

		if (blob.size() != expectedSize)
		{
			NLE_CORE_WARN("Blob size mismatch for component ID {}: expected {}, got {}", STRFROM(compID), expectedSize, blob.size());
			return;
		}


		auto binDeserializeIt = m_ComponentBinaryDeserializer.find(compID);
		if (binDeserializeIt != m_ComponentBinaryDeserializer.end())
		{
			void* component = binDeserializeIt->second(blob, expectedSize);

			auto nameAssigner = m_NameIDAssigner.find(compID);
			if (nameAssigner != m_NameIDAssigner.end())
			{
				nameAssigner->second(component, nameID);

				//m_ComponentDestroyer[compID](component);


			}

			adderIt->second(component, registry, id);

			m_ComponentDestroyer[compID](component);


		}
		else
		{
			NLE_CORE_THROW("No deserializer found for component: {0}", STRFROM(compID));
		}
	}



	//void NComponentFactory::AddComponentFromBinary(uint32_t compNameID, const std::vector<uint8_t>& blob, NRegistry* registry, EntityID id) const
	//{
	//	AddComponentFromBinary(m_ComponentNamesToID[compNameID], blob, registry, id);
	//}


	//BaseComponent& NComponentFactory::CreateUniqueComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
	//{
	//	BaseComponent* component = m_ComponentCreator.at(STRID(componentName))();

	//	auto it = m_componentDeserializer.find(STRID(componentName));
	//	if (it != m_componentDeserializer.end()) {
	//		it->second(component, componentData, id);
	//	}
	//	else
	//	{
	//		NLE_CORE_WARN("No Read function found for {0}", component->Name());
	//	}

	//	return *component;
	//}


}
