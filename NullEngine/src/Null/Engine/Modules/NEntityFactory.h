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
#include "../../Tools/ArchetypeDefinitions.h"


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


		ArchetypeContainer m_Archetypes;

	public:
		class ArchetypeHelper
		{
		public:
			ArchetypeHelper();
			~ArchetypeHelper();

			static void AddArchetype(uint32_t archetypeNameID, ArchetypeContainer& archetypeContainer);
			static void AddArchetypeChild(uint32_t archetypeNameID, uint32_t childNameID, ArchetypeContainer& archetypeContainer);

			static bool HasArchetype(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer);
			static bool ArchetypeHasComponent(uint32_t archetypeNameID, uint32_t compNameID, const ArchetypeContainer& archetypeContainer);
			static bool ChildHasComponent(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const ArchetypeContainer& archetypeContainer);


			static void UpdateArchetype(uint32_t archetypeNameID, uint32_t compNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer);
			static void UpdateArchetypeWithChild(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer);

			//void HandleChildren(


			static const std::unordered_map<uint32_t, std::vector<uint8_t>>& GetArchetypeComponenetList(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer);
			static const std::unordered_map<uint32_t, std::vector<uint8_t>>& GetArchetypeChildComponenetList(uint32_t archetypeNameID, uint32_t childNameID, const ArchetypeContainer& archetypeContainer);
			static const ArchetypeContainer& GetChildren(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer);
		private:
		};


		//struct ArchetypeData
		//{
		//	std::unordered_map<std::string, std::vector<uint8_t>> components;
		//	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<uint8_t>>> children;

		//	ArchetypeData() : components(), children()
		//	{

		//	}
		//};

		void CloneChild_Rec(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData, const ArchetypeContainer& archetypeDef);
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

		void RegisterArchetype(uint32_t archetypeNameID);


		Entity CreateEntity(const JSON& entityData, NRegistry* registry);

		Entity CreateEntity(NRegistry* registry);

		void CloneOrCreateArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData);
		void CloneChild(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData);


		void UpdateArchetype(uint32_t archetypeNameID, uint32_t compNameID, std::vector<uint8_t> component);
		void UpdateArchetypeWithChild(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer);


		bool HasArchetype(uint32_t archetypeNameID ) const;

		bool ArchetypeHasComponent(uint32_t archetypeNameID, uint32_t compNameID) const;

		void ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, bool isChild= false);
		void ReadChildrenFromArchetype(Entity& parentEntity, uint32_t archetypeNameID, const nlohmann::json& childrenData, NRegistry* registry,
									  NComponentFactory* componentFactory, ArchetypeContainer& archetypeContainer);

		void CloneComponents(NComponentFactory* componentFactory, uint32_t archetypeNameID, NRegistry* registry, EntityID id);
		void CloneChildComponents(NComponentFactory* componentFactory, uint32_t archetypeNameID, uint32_t childNameID, NRegistry* registry, EntityID id, const ArchetypeContainer& archetypeContainer);

		void RegisterToScripAPI(sol::state& lua) override;




	};

}