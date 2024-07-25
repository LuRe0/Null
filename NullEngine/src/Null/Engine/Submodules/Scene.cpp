
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

	void Scene::Update(float dt)
	{

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