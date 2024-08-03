#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NEntityFactory.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{

	class Entity;

	class NLE_API NEntityFactory : public IModule
	{
	public:

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void RegisterArchetype(const std::string& archetypeName);


		Entity CreateEntity(const JSON& entityData, NRegistry* registry);

		Entity CreateEntity(NRegistry* registry);

		void CloneOrCreateArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData);


		void UpdateArchetype(const std::string& archetypeName, BaseComponent* component);

		bool HasArchetype(const std::string& archetypeName) const;

		bool ArchetypeHasComponent(const std::string& archetypeName, BaseComponent* component) const;

		void ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry);

		void CloneComponents(NComponentFactory* componentFactory, const std::string& archetype, NRegistry* registry, EntityID id);

		void RegisterToScripAPI(sol::state& lua) override;

	private:
		std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>> m_Archetypes;
	};

}