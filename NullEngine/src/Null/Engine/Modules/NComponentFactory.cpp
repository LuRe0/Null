
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


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	std::unordered_map<std::string, std::function<void(void*, const nlohmann::json&, NRegistry*, EntityID id)>> NComponentFactory::m_componentReader;
	std::unordered_map<std::string, std::function<nlohmann::json(BaseComponent*)>> NComponentFactory::m_componentWriter;
	std::unordered_map<std::string, std::function<BaseComponent* ()>> NComponentFactory::m_ComponentCreator;
	std::unordered_map<std::string, uint32_t> NComponentFactory::m_ComponentNamesToID;
	std::unordered_map<size_t, std::function<void(Entity&)>> NComponentFactory::m_ComponentInspector;
	std::unordered_map<std::string, std::unordered_map<uint32_t, std::function<sol::object(Entity&, sol::this_state)>>> NComponentFactory::m_TypeRegistry;

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
	nlohmann::json NComponentFactory::WriteComponent(BaseComponent* component) const
	{
		auto it = m_componentWriter.find(component->Name());
		if (it != m_componentWriter.end())
		{
			return it->second(component);
		}

		NLE_CORE_WARN("No Write function found for {0}", component->Name());

		return JSON();
	}

	void NComponentFactory::ViewComponent(Entity entity, uint32_t componentId) const
	{
		auto it = m_ComponentInspector.find(componentId);
		if (it != m_ComponentInspector.end()) {
			it->second(entity);
		}
		else
			NLE_CORE_WARN("No Inspector function found for {0}", componentId);
	}

	BaseComponent& NComponentFactory::CreateComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
	{
		BaseComponent* component = m_ComponentCreator.at(componentName)();

		auto it = m_componentReader.find(componentName);
		if (it != m_componentReader.end()) {
			it->second(component, componentData, registry, id);
		}
		else
		{
			NLE_CORE_WARN("No Read function found for {0}", component->Name());
		}

		return *component;
	}

	void NComponentFactory::CloneComponent(const std::string& componentName, BaseComponent* component, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
	{
		auto it = m_componentReader.find(componentName);
		if (it != m_componentReader.end()) {
			it->second(component, componentData, registry, id);
		}
		else
		{
			NLE_CORE_WARN("No Read function found for {0}", component->Name());
		}
	}

	void NComponentFactory::CreateUniqueComponent(const std::string& componentName, const nlohmann::json& componentData, NRegistry* registry, EntityID id) const
	{
		BaseComponent* component = m_ComponentCreator.at(componentName)();

		auto it = m_componentReader.find(componentName);
		if (it != m_componentReader.end()) {
			it->second(component, componentData, registry, id);
		}
		else
		{
			NLE_CORE_WARN("No Read function found for {0}", component->Name());
		}

		delete component;
	}

	

}
