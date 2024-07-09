
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
#include "Null/Engine/Entities/Entity.h"
#include "Null/Tools/Trace.h"



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
		for (const auto& transitionData : sceneData["transitions"]) {

			Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

			m_Transitions.push_back(transition);
		}

		for (const auto& entityData : sceneData["entities"]) 
		{
			const std::string name = entityData["name"];

			Entity newEntity(m_EntityManager.CreateEntity(name));

			newEntity.SetName(name);
	
			newEntity.Read(entityData);
		}

	}




	void Scene::Init()
	{
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

	EntityID Scene::EntityManager::CreateEntity(const std::string& name)
	{
		EntityID entity = 0;
		if (m_freeEntities.empty()) {
			entity = ++m_NextEntity;
		}
		else {
			entity = m_freeEntities.back();
			m_freeEntities.pop_back();
		}

		(entity == 0) ? NLE_CORE_ERROR("Failed to create Entity") : NLE_CORE_TRACE("Succesfully created entity ID: {0}, Name: {1}", entity, name);


		m_EntityNames[entity] = name;
		// Initialize new entity's component data
		//componentMasks[entity] = 0;
		return entity;
	}
}