#pragma once

//------------------------------------------------------------------------------
//
// File Name:	SpriteSourceSetSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct SpriteSourceSetComponent;

	class NLE_API SpriteSourceSetSystem: public ISystem < SpriteSourceSetSystem >
	{
	public:

		SpriteSourceSetSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:
		struct SpriteSourceSet
		{
			uint32_t spriteSourceID = -1; // Index in the system-wide storage for sprite source sets
			uint32_t emissiveSpriteSourceID = -1; // Index for emissive sprite source, if applicable
		};
		static void CreateSpriteSourceSetComponent(void* component, const nlohmann::json& json);
		static void AddSpriteSourceSetComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteSpriteSourceSetComponent(const void* component);
		static JSON DiffSpriteSourceSetComponent(const void* base, const void* modified);

		void ViewSpriteSourceSetComponent(Entity& entity);


		std::vector<std::vector<SpriteSourceSet>> m_SpriteSourceSets;
	};
}
