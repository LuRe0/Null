
//------------------------------------------------------------------------------
//
// File Name:	TilemapRenderSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "TilemapRenderSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include "Null/Engine/Submodules/Events/IEvents.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	TilemapRenderSystem::TilemapRenderSystem()
	{
		Require<TransformComponent>();
		Require<TilemapComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<TilemapComponent>(CreateTilemapComponent,
			[this](Entity& id) { this->ViewTilemapComponent(id); }, WriteTilemapComponent);
	}

	void TilemapRenderSystem::Load()
	{
		ISystem::Load();
	}

	void TilemapRenderSystem::Init()
	{
		ISystem::Init();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &TilemapRenderSystem::OnEntityCreated, eventManager, EventPriority::Low);

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		InitTilemap(GetSystemEntities(), registry);
	}

	void TilemapRenderSystem::Update(float dt)
	{

	}

	void TilemapRenderSystem::Render()
	{
	}

	void TilemapRenderSystem::Unload()
	{

	}

	void TilemapRenderSystem::Shutdown()
	{
	}

	void TilemapRenderSystem::RegisterToScripAPI(sol::state& lua)
	{

	}


	void TilemapRenderSystem::CreateTilemapComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		auto* comp = static_cast<TilemapComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Name = jsonWrapper.GetString("name", "");
			comp->m_PathName = jsonWrapper.GetString("path", "");
			comp->m_TileScale = jsonWrapper.GetFloat("scale", 1.0f);

			NLE_CORE_ASSERT(!comp->m_PathName.empty(), "Cannot fine tilemap of name {0}", comp->m_PathName);
		}

		// Add or update component in registry
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		componentFactory->AddOrUpdate<TilemapComponent>(id, comp, registry, comp->m_Name, comp->m_PathName, comp->m_TileScale);
	}


	JSON TilemapRenderSystem::WriteTilemapComponent(BaseComponent* component)
	{

		return JSON();
	}

	void TilemapRenderSystem::ViewTilemapComponent(Entity& entity)
	{
	}

	bool TilemapRenderSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
			return true;

		InitTilemap(entityList, registry);

		return true;
	}

	void TilemapRenderSystem::InitTilemap(const std::vector<EntityID>& entityList, NRegistry* registry)
	{
		for (const auto entityId : entityList)
		{
			TilemapComponent& tilemapComp = registry->GetComponent<TilemapComponent>(entityId);
			tilemapComp.m_Tilemap = std::make_unique<Tilemap>(tilemapComp.m_PathName, tilemapComp.m_Name);

			tilemapComp.m_Tilemap.get()->CreateTilemap(tilemapComp.m_TileScale, entityId);
		}

	}
}