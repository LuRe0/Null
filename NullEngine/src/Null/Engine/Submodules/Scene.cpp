
//------------------------------------------------------------------------------
//
// File Name:	Scene.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Scene.h"
#include "Null/Tools/JsonWrapper.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


namespace NULLENGINE
{

	void Scene::Load(const std::string& name, const JSON& sceneData)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		for (const auto& transitionData : sceneData["transitions"]) {

			Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

			m_Transitions.push_back(transition);
		}

		for (const auto& entityData : sceneData["entities"]) 
		{
			JsonWrapper jsonWrapper(entityData);

			Entity entity = entityFactory->CreateEntity(entityData, registry);
	

			if (entityData.contains("components"))
			{
				const std::string& name = jsonWrapper.GetString("name", "Entity(" + std::to_string(entity.GetID()) + ")");

				entity.SetName(name);

				for (const auto& [componentName, componentData] : entityData["components"].items())
				{
					componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, entity.GetID());
				}

			}

			m_Entities.push_back(entity);

		}

	}




	void Scene::Init()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto& entity : m_Entities)
		{
			registry->AddEntityToSystem(entity.GetID());
		}
	}

	EntityID Scene::AddEntity(const std::string& name)
	{
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		Entity entity = entityFactory->CreateEntity(registry);

		entity.SetName(name);

		m_Entities.push_back(entity);

		eventManager->QueueEvent(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		return entity.GetID();
	}

	Entity& Scene::GetEntity(const EntityID& entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);

		NLE_CORE_ASSERT(it != m_Entities.end(), "Entity not found", entityID);

		return *it;
	}


	void Scene::RemoveEntity(size_t pos)
	{
		std::swap(m_Entities.back(), m_Entities[pos]);
		m_Entities.pop_back();
	}

	void Scene::DeleteEntity(EntityID entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);
		if ( it != m_Entities.end())
		{
			it->SetIsDestroyed(true);
		}
	}


	void Scene::Update(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		for (size_t i = 0; i < m_Entities.size(); i++)
		{
			if (i < 0)
				return;

			if (m_Entities[i].GetIsDestroyed())
			{
				eventManager->QueueEvent(std::make_unique<EntityDestroyedEvent>(m_Entities[i].GetID()));
				
				RemoveEntity(i);

				--i;
			}
		}
		for (const auto& entity : m_Entities)
		{

		}
	}

	void Scene::Render()
	{

	}

	void Scene::Unload()
	{
	}

	void Scene::Shutdown()
	{
	}

}