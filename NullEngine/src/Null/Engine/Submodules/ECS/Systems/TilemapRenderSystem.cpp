
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
#include "NIncludes.h"


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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<TilemapComponent>(CreateTilemapComponent,
			[this](Entity& id) { this->ViewTilemapComponent(id); }, WriteTilemapComponent,
			AddTilemapComponent, DiffTilemapComponent);
	}

	void TilemapRenderSystem::Load()
	{
		ISystem::Load();
	}

	void TilemapRenderSystem::Init()
	{
		ISystem::Init();
		NEventManager* eventManager =   NEventManager::Instance();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &TilemapRenderSystem::OnEntityCreated, eventManager, EventPriority::Low);

		NRegistry* registry = NRegistry::Instance();

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


	void TilemapRenderSystem::CreateTilemapComponent(void* component, const nlohmann::json& json)
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
		//NComponentFactory* componentFactory = NComponentFactory::Instance();
	}

	void TilemapRenderSystem::AddTilemapComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<TilemapComponent*>(component);
		componentFactory->AddOrUpdate<TilemapComponent>(id, comp, registry, comp->m_Name, comp->m_PathName, comp->m_TileScale);
	}


	JSON TilemapRenderSystem::WriteTilemapComponent(const void* component)
	{
		nlohmann::json json;

		auto& tilemap = *static_cast<const TilemapComponent*>(component);

		//if (!tilemap.componentFlags.IsSet(ComponentFlags_Serialized))
		//	return json;

		json["Tilemap"]["name"] = tilemap.m_Name;
		json["Tilemap"]["path"] = tilemap.m_PathName;
		json["Tilemap"]["tileScale"] = tilemap.m_TileScale;

		return json;
	}


	void TilemapRenderSystem::ViewTilemapComponent(Entity& entity)
	{
	}


	JSON TilemapRenderSystem::DiffTilemapComponent(const void* base, const void* modified)
	{
		nlohmann::json diff;

		auto& a = *static_cast<const TilemapComponent*>(base);
		auto& b = *static_cast<const TilemapComponent*>(modified);

		if (a.m_Name != b.m_Name)
			diff["name"] = b.m_Name;

		if (a.m_PathName != b.m_PathName)
			diff["path"] = b.m_PathName;

		if (a.m_TileScale != b.m_TileScale)
			diff["tileScale"] = b.m_TileScale;

		return diff;
	}

	bool TilemapRenderSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

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