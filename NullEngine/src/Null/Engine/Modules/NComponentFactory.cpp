
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
	void NComponentFactory::Load()
	{
		//AddCreateFunction<TransformComponent>(CreateTransformComponent);
		//AddCreateFunction<SpriteComponent>(CreateSpriteComponent);
		//AddCreateFunction<Rigidbody2DComponent>(CreateRigidbody2DComponent);
		//AddCreateFunction<BoxCollider2DComponent>(CreateBoxCollider2DComponent);
		//AddCreateFunction<AnimationComponent>(CreateAnimationComponent);
	}

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
