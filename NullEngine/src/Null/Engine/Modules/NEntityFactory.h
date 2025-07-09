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

	class NLE_API NEntityFactory : public ModuleBase<NEntityFactory>
	{
	private:

		struct EntityDefinition
		{
			std::string referencedArchetype = ""; // empty if not a reference
			std::unordered_map<std::string, BaseComponent*> components = std::unordered_map<std::string, BaseComponent*>();
			std::unordered_map<std::string, EntityDefinition> children = std::unordered_map<std::string, EntityDefinition>();
		};

		typedef std::unordered_map<std::string, EntityDefinition> ArchetypeContainer;


		ArchetypeContainer m_Archetypes;


		class ArchetypeHelper
		{
		public:
			ArchetypeHelper();
			~ArchetypeHelper();

			static void AddArchetype(const std::string& archetype, NEntityFactory::ArchetypeContainer& archetypeContainer);
			static void AddArchetypeChild(const std::string& archetype, const std::string& child, NEntityFactory::ArchetypeContainer& archetypeContainer);

			static bool HasArchetype(const std::string& archetype, const NEntityFactory::ArchetypeContainer& archetypeContainer);
			static bool ArchetypeHasComponent(const std::string& archetype, BaseComponent* component, const NEntityFactory::ArchetypeContainer& archetypeContainer);
			static bool ChildHasComponent(const std::string& archetype, const std::string& child, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer);


			static void UpdateArchetype(const std::string& archetypeName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer);
			static void UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer);

			//void HandleChildren(


			static std::unordered_map<std::string, BaseComponent*>& GetArchetypeComponenetList(const std::string& archetypeName, NEntityFactory::ArchetypeContainer& archetypeContainer);
			static std::unordered_map<std::string, BaseComponent*>& GetArchetypeChildComponenetList(const std::string& archetypeName, const std::string& childName, ArchetypeContainer& archetypeContainer);
			static ArchetypeContainer& GetChildren(const std::string& archetypeName, NEntityFactory::ArchetypeContainer& archetypeContainer);
		private:
		};


		//struct ArchetypeData
		//{
		//	std::unordered_map<std::string, BaseComponent*> components;
		//	std::unordered_map<std::string, std::unordered_map<std::string, BaseComponent*>> children;

		//	ArchetypeData() : components(), children()
		//	{

		//	}
		//};

		void CloneChild_Rec(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData, ArchetypeContainer& archetypeDef);
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
		void UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer);


		bool HasArchetype(const std::string& archetypeName) const;

		bool ArchetypeHasComponent(const std::string& archetypeName, BaseComponent* component) const;

		void ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, bool isChild= false);
		void ReadChildrenFromArchetype(Entity& parentEntity, const std::string& archetype, const nlohmann::json& childrenData, NRegistry* registry,
									  NComponentFactory* componentFactory, NEntityFactory::ArchetypeContainer& archetypeContainer);

		void CloneComponents(NComponentFactory* componentFactory, const std::string& archetype, NRegistry* registry, EntityID id);
		void CloneChildComponents(NComponentFactory* componentFactory, const std::string& archetype, const std::string& childName, NRegistry* registry, EntityID id, NEntityFactory::ArchetypeContainer& archetypeContainer);

		void RegisterToScripAPI(sol::state& lua) override;


	};

}