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
		void RuntimeUpdate(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void RegisterArchetype(const std::string& archetypeName);


		Entity CreateEntity(const JSON& entityData, NRegistry* registry);

		Entity CreateEntity(NRegistry* registry);

		void CloneOrCreateArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData);
		void CloneChild(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData);


		void UpdateArchetype(const std::string& archetypeName, BaseComponent* component);
		void UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component);


		bool HasArchetype(const std::string& archetypeName) const;

		bool ArchetypeHasComponent(const std::string& archetypeName, BaseComponent* component) const;

		void ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, bool isChild= false);
		void ReadChildrenFromArchetype(Entity& parentEntity, const std::string& archetype, const nlohmann::json& childrenData, NRegistry* registry,
									  NComponentFactory* componentFactory);

		void CloneComponents(NComponentFactory* componentFactory, const std::string& archetype, NRegistry* registry, EntityID id);
		void CloneChildComponents(NComponentFactory* componentFactory, const std::string& archetype, const std::string& childName, NRegistry* registry, EntityID id);

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		class ArchetypeManager
		{
		public:
			ArchetypeManager();
			~ArchetypeManager();

			void AddArchetype(const std::string& archetype);
			void AddArchetypeChild(const std::string& archetype, const std::string& child);

			bool HasArchetype(const std::string& archetype) const;
			bool ArchetypeHasComponent(const std::string& archetype, BaseComponent* component) const;
			bool ChildHasComponent(const std::string& archetype, const std::string& child, BaseComponent* component) const;


			void UpdateArchetype(const std::string& archetypeName, BaseComponent* component);
			void UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component);


			std::unordered_map<std::string, BaseComponent*>& GetArchetypeComponenetList(const std::string& archetypeName);
			std::unordered_map<std::string, BaseComponent*>& GetArchetypeChildComponenetList(const std::string& archetypeName, const std::string& childName);
			std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>>& GetChildren(const std::string& archetypeName);
		private:
				std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>> m_ArchetypeComponents;
				std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>>> m_ChildrenComponent;
		};

		//struct ArchetypeData
		//{
		//	std::unordered_map<std::string, BaseComponent*> components;
		//	std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>> children;

		//	ArchetypeData() : components(), children()
		//	{

		//	}
		//};
	
		//std::unordered_map<std::string, ArchetypeData> m_Archetypes;

		ArchetypeManager m_ArchetypeManager;
	};

}