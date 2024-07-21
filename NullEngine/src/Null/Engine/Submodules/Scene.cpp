
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
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"




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

		for (const auto& transitionData : sceneData["transitions"]) {

			Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

			m_Transitions.push_back(transition);
		}

		for (const auto& entityData : sceneData["entities"]) 
		{
			JsonWrapper jsonWrapper(entityData);

			EntityID entityID = entityFactory->CreateEntity(entityData, registry);
	
			m_Entities.push_back(entityID);
		}

	}




	void Scene::Init()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : m_Entities)
			registry->AddEntityToSystem(entityId);
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