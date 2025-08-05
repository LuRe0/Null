
//------------------------------------------------------------------------------
//
// File Name:	NEntityFactory.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NEntityFactory.h"
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"
#include "sol/sol.hpp"
#include "nlohmann/json.hpp"
#include "NSceneManager.h"
#include "../Submodules/Scene.h"
#include <NIncludes.h>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void NEntityFactory::Load()
    {
    }

    void NEntityFactory::Init()
    {
    }

    void NEntityFactory::Update(float dt)
    {
    }

    void NEntityFactory::RuntimeUpdate(float dt)
    {
    }

    void NEntityFactory::Unload()
    {
        for (auto& archetype : m_Archetypes)
        {
            archetype.second.Destroy();
		}

        m_Archetypes.clear();
    }

    void NEntityFactory::Shutdown()
    {
    }
    void NEntityFactory::RegisterArchetype(uint32_t archetypeNameID)
    {


        ArchetypeHelper::AddArchetype(archetypeNameID, m_Archetypes);

    }

    Entity NEntityFactory::CreateEntity(const JSON& entityData, NRegistry* registry)
    {
        Entity newEntity(registry->CreateEntity(), registry);

        JsonReader jsonWrapper(entityData);

        if (!jsonWrapper.Empty())
        {

            NComponentFactory* componentFactory = NComponentFactory::Instance();

            const std::string& archetypeNameID = jsonWrapper.GetString("archetype", "");

            //CloneOrCreateArchetype(archetype, newEntity, componentFactory, registry, entityData);
        }

        return newEntity;

    }

    Entity NEntityFactory::CreateEntity(NRegistry* registry)
    {
        return  Entity(registry->CreateEntity(), registry);;
    }

    void NEntityFactory::CloneOrCreateArchetype(const std::string& archetypeNameID, Entity& newEntity,
        NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData)
    {
        //if this is reading from
        //if (!archetype.empty())
        //{
        //    JsonReader jsonWrapper(entityData);

        //    newEntity.SetArchetype(archetype);

        //    if (!HasArchetype(archetype))
        //    {

        //        RegisterArchetype(archetype);

        //        ReadArchetype(archetype, newEntity, componentFactory, registry);

        //        if (newEntity.GetName().empty())
        //        {
        //            const std::string& name = jsonWrapper.GetString("name", "");

        //            if (!name.empty())
        //                newEntity.SetName(name);
        //            else
        //                newEntity.SetName(archetype + " (Clone)");
        //        }

        //        return;
        //    }

        //    CloneComponents(componentFactory, archetype, registry, newEntity.GetID());

        //    CloneChild(newEntity, archetype, componentFactory, registry, JSON());

        //    if (newEntity.GetName().empty())
        //    {
        //        const std::string& name = jsonWrapper.GetString("name", "");

        //        if (!name.empty())
        //            newEntity.SetName(name);
        //        else
        //            newEntity.SetName(archetype + " (Clone)");
        //    }
        //}
    }

    void NEntityFactory::CloneChild(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData)
    {
        NEventManager* eventManager =   NEventManager::Instance();

        //const auto& children = ArchetypeHelper::GetChildren(parentArchetype, m_Archetypes);

        //for (const auto& child : children)
        //{
        //    Entity childEntity = CreateEntity(registry);

        //    childEntity.SetName(child.first);
        //    childEntity.SetParentArchetype(parentArchetype);
        //    CloneChildComponents(componentFactory, parentArchetype, child.first, registry, childEntity.GetID(), m_Archetypes);

        //    registry->AddComponent<ParentComponent>(childEntity.GetID(), pEntity.GetID());
        //    auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(pEntity.GetID());
        //    childrenComponent.m_Children.push_back(childEntity.GetID());

        //    eventManager->TriggerEvent(EntityLoadedEvent(childEntity));

        //    CloneChild_Rec(childEntity, child.first, componentFactory, registry, entityData, children);
        //}
    }

    void NEntityFactory::CloneChild_Rec(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData, const ArchetypeContainer& archetypeDef)
    {
        //NEventManager* eventManager =   NEventManager::Instance();

        //const auto& children = ArchetypeHelper::GetChildren(parentArchetype, archetypeDef);

        //for (auto& child : children)
        //{
        //    Entity childEntity = CreateEntity(registry);

        //    childEntity.SetName(child.first);
        //    childEntity.SetParentArchetype(parentArchetype);
        //    CloneChildComponents(componentFactory, parentArchetype, child.first, registry, childEntity.GetID(), archetypeDef);

        //    registry->AddComponent<ParentComponent>(childEntity.GetID(), pEntity.GetID());
        //    auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(pEntity.GetID());
        //    childrenComponent.m_Children.push_back(childEntity.GetID());

        //    eventManager->TriggerEvent(EntityLoadedEvent(childEntity));

        //    CloneChild_Rec(childEntity, child.first, componentFactory, registry, entityData, children);
        //}
    }



    void NEntityFactory::UpdateArchetype(uint32_t archetypeNameID, uint32_t compNameID, std::vector<uint8_t> component)
    {
       NLE_CORE_ASSERT(!ArchetypeHasComponent(archetypeNameID, compNameID), "Component previously added to archetype!", STRFROM(archetypeNameID));
        
       ArchetypeHelper::UpdateArchetype(archetypeNameID, compNameID, component, m_Archetypes);
    }


    void NEntityFactory::UpdateArchetypeWithChild(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer)
    {
        ArchetypeHelper::UpdateArchetypeWithChild(archetypeNameID, childNameID, compNameID, component, archetypeContainer);
    }

    bool NEntityFactory::HasArchetype(uint32_t archetypeNameID) const
    {
        return  ArchetypeHelper::HasArchetype(archetypeNameID, m_Archetypes);
    }
    
    bool NEntityFactory::ArchetypeHasComponent(uint32_t archetypeNameID, uint32_t compNameID) const
    {
        return ArchetypeHelper::ArchetypeHasComponent(archetypeNameID, compNameID, m_Archetypes);
    }

    void NEntityFactory::ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, bool isChild)
    {
        //std::string filePath = std::string("../Assets/Archetypes/") + filename + std::string(".ent");

        //// Open the JSON file
        //std::ifstream inputFile(filePath);
        //if (!inputFile.is_open()) {
        //    NLE_ERROR("Error: Could not open file");
        //    return;
        //}

        //// Parse the JSON file
        //JSON entityData;

        //try {
        //    inputFile >> entityData;
        //}
        //catch (JSON::parse_error& e) {
        //    NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
        //    return;
        //}

        //uint32_t archetypeNameID = filename;

        ////auto components = entityData["components"];

        //JsonReader archetypeWrapper(entityData);

        //const std::string& name = archetypeWrapper.GetString("Name", "");

        //if (entity.GetName().empty())
        //    entity.SetName(name);

        //for (const auto& [componentName, componentData] : entityData["components"].items())
        //{
        //    BaseComponent& component = componentFactory->CreateComponent(componentName + "Component", componentData, registry, entity.GetID());

       
        //    UpdateArchetype(archetypeName, &component);

        //    if (isChild)
        //        UpdateArchetypeWithChild(entity.m_ParentArchetype, entity.m_Name, &component, m_Archetypes);
        //}


        //if (entityData.contains("children"))
        //{
        //    ReadChildrenFromArchetype(entity, archetypeName, entityData["children"], registry, componentFactory, m_Archetypes);
        //}
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="parentEntity"></param>
    /// <param name="archetype"></param>
    /// <param name="childrenData"></param>
    /// <param name="registry"></param>
    /// <param name="componentFactory"></param>
    //void NEntityFactory::ReadChildrenFromArchetype(Entity& parentEntity, const std::string& archetypeNameID, const nlohmann::json& childrenData, NRegistry* registry,
    //                                              NComponentFactory* componentFactory, ArchetypeContainer& archetypeContainer)
    //{
    //    //NEventManager* eventManager =   NEventManager::Instance();

    //    //for (const auto& childData : childrenData)
    //    //{
    //    //    Entity childEntity = CreateEntity(childData, registry);
    //    //    uint32_t childNameID = childData.contains("name") ? childData["name"].get<std::string>() : "ChildEntity(" + std::to_string(childEntity.GetID()) + ")";
    //    //    std::string childArchetypeName = childName;
    //    //    childEntity.SetName(childName);
    //    //    childEntity.SetParentArchetype(parentEntity.m_Archetype);

    //    //    if (childData.contains(archetype))
    //    //    {
    //    //        JsonReader archetypeWrapper(childData);

    //    //        const std::string& archetypeNameID = archetypeWrapper.GetString("archetype", "");

    //    //        ReadArchetype(archetype, childEntity, componentFactory, registry);
    //    //        childArchetypeName = archetype;
    //    //    }


    //    //    if (childData.contains("components"))
    //    //    {
    //    //        for (const auto& [componentName, componentData] : childData["components"].items())
    //    //        {
    //    //            BaseComponent& component = componentFactory->CreateComponent(componentName + "Component", componentData, registry, childEntity.GetID());
    //    //            UpdateArchetypeWithChild(archetype, childName, &component, archetypeContainer);
    //    //        }
    //    //    }

    //    //    CloneChildComponents(componentFactory, archetype, childName, registry, childEntity.GetID(), archetypeContainer);

    //    //    registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
    //    //    auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
    //    //    childrenComponent.m_Children.push_back(childEntity.GetID());

    //    //    if (childData.contains("children"))
    //    //    {
    //    //        ReadChildrenFromArchetype(childEntity, childArchetypeName, childData["children"], registry, componentFactory, archetypeContainer[archetype].children);
    //    //    }

    //    //    eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));
    //    //    eventManager->TriggerEvent(EntityLoadedEvent(childEntity));
    //    //}
    //}

    /// <summary>
    /// 
    /// </summary>
    /// <param name="componentFactory"></param>
    /// <param name="archetype"></param>
    /// <param name="registry"></param>
    /// <param name="id"></param>
    //void NEntityFactory::CloneComponents(NComponentFactory* componentFactory, const std::string& archetypeNameID, NRegistry* registry, EntityID id)
    //{
    //    const std::unordered_map<uint32_t, std::vector<uint8_t>>& componentList = ArchetypeHelper::GetArchetypeComponenetList(archetype, m_Archetypes);

    //    for (auto component : componentList)
    //    {
    //        componentFactory->AddComponent(component.first, component.second, registry, id);
    //    }
    //}

    ///// <summary>
    ///// 
    ///// </summary>
    ///// <param name="componentFactory"></param>
    ///// <param name="archetype"></param>
    ///// <param name="childName"></param>
    ///// <param name="registry"></param>
    ///// <param name="id"></param>
    //void NEntityFactory::CloneChildComponents(NComponentFactory* componentFactory, const std::string& archetypeNameID, uint32_t childNameID, NRegistry* registry, EntityID id, const ArchetypeContainer& archetypeContainer)
    //{
    //    const std::unordered_map<uint32_t, std::vector<uint8_t>>& componentList = ArchetypeHelper::GetArchetypeChildComponenetList(archetype,childName, archetypeContainer);

    //    for (auto component : componentList)
    //    {
    //        componentFactory->AddComponentFromBinary(component.first, component.second, registry, id);
    //    }
    //}

    /// <summary>
    /// 
    /// </summary>
    /// <param name="lua"></param>
    void NEntityFactory::RegisterToScripAPI(sol::state& lua)
    {
        lua.new_usertype<Entity>("Entity",
            sol::no_constructor,
            sol::meta_function::to_string, [](Entity& e) { return "Entity: " + std::to_string(e.m_ID) + ", " + STRFROM(e.Get<NameComponent>().nameID); },
            sol::call_constructor, sol::constructors<Entity&>(),
            "get_component", [](Entity& entity, const sol::table& comp, sol::this_state s) -> sol::object
            {
                sol::state_view lua(s);
                auto typeID = NComponentFactory::GetIdType(comp);

                auto hasComp = NComponentFactory::InvokeSolFunctions(typeID, "has_component", entity, s);
                if (!hasComp.valid() || !hasComp.as<bool>())
                    return sol::make_object(lua, sol::lua_nil);  // nil

                auto component = NComponentFactory::InvokeSolFunctions(typeID, "get_component", entity, s);
                return component.valid()
                    ? component.as<sol::object>()
                    : sol::make_object(lua, sol::lua_nil);  // fail-safe
            },
            "add_component", [](Entity& entity, const sol::table& comp, sol::this_state s)
            {
                //const auto has_comp
                auto component = NComponentFactory::InvokeSolFunctions(NComponentFactory::GetIdType(comp), "add_component", entity, s);

                return component ? component.as<sol::reference>() : sol::lua_nil_t{};
            },
            "has_component", [](Entity& entity, const sol::table& comp, sol::this_state s)
            {
                //const auto has_comp
                auto hasComp = NComponentFactory::InvokeSolFunctions(NComponentFactory::GetIdType(comp), "has_component", entity,s);

                return hasComp ? hasComp.as<bool>() : false;
            },
            "remove_component", [](Entity& entity, const sol::table& comp, sol::this_state s)
            {
                //const auto has_comp
                auto hasComp = NComponentFactory::InvokeSolFunctions(NComponentFactory::GetIdType(comp), "remove_component", entity,s);

                return hasComp ? hasComp.as<bool>() : false;
            },
            "destroy", [](Entity& entity)
            {
                entity.Add<DestroyedComponent>();
            },
            "is_destroyed", [](Entity& entity)
            {
				return entity.Has<DestroyedComponent>();
            },
            "name", [](Entity& entity)
            {
                return STRFROM(entity.Get<NameComponent>().nameID);
            },
        	"id", &Entity::m_ID  
        );

    }

    NEntityFactory::ArchetypeHelper::ArchetypeHelper()
    {
    }

    NEntityFactory::ArchetypeHelper::~ArchetypeHelper()
    {
    }

    void NEntityFactory::ArchetypeHelper::AddArchetype(uint32_t archetypeNameID, ArchetypeContainer& archetypeContainer)
    {
		if (!archetypeContainer.contains(archetypeNameID))
		{
            EntityDefinition def;

            archetypeContainer.emplace(archetypeNameID, def);

			return;
		}

        NLE_CORE_INFO("Archetype {0} already exists!", STRFROM(archetypeNameID));
    }

    void NEntityFactory::ArchetypeHelper::AddArchetypeChild(uint32_t archetypeNameID, uint32_t childNameID, ArchetypeContainer& archetypeContainer)
    {
        if (archetypeContainer.contains(archetypeNameID))
        {
            if (!archetypeContainer[archetypeNameID].children.contains(childNameID))
            {
                EntityDefinition def;
                archetypeContainer[archetypeNameID].children.emplace(childNameID, def);
                return;
            }

            NLE_CORE_INFO("Child {0} already exists!", STRFROM(childNameID));
        }

        NLE_CORE_INFO("Archetype {0} does not exists!", STRFROM(archetypeNameID));

    }

    bool NEntityFactory::ArchetypeHelper::HasArchetype(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer)
    {
        return archetypeContainer.contains(archetypeNameID);
    }

    bool NEntityFactory::ArchetypeHelper::ArchetypeHasComponent(uint32_t archetypeNameID, uint32_t compNameID, const ArchetypeContainer& archetypeContainer)
    {
        const std::unordered_map<uint32_t, std::vector<uint8_t>>& componentList = archetypeContainer.at(archetypeNameID).components;

        return componentList.contains(compNameID);
    }

    bool NEntityFactory::ArchetypeHelper::ChildHasComponent(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const ArchetypeContainer& archetypeContainer)
    {
        const std::unordered_map<uint32_t, std::vector<uint8_t>> componentList = archetypeContainer.at(archetypeNameID).children.at(childNameID).components;

        return componentList.contains(compNameID);
    }

    void NEntityFactory::ArchetypeHelper::UpdateArchetype(uint32_t archetypeNameID, uint32_t compNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer)
    {
        archetypeContainer.at(archetypeNameID).components.emplace(std::make_pair(compNameID, component));
    }

    void NEntityFactory::ArchetypeHelper::UpdateArchetypeWithChild(uint32_t archetypeNameID, uint32_t compNameID, uint32_t childNameID, const std::vector<uint8_t>& component, ArchetypeContainer& archetypeContainer)
    {
        archetypeContainer[archetypeNameID].children[childNameID].components.emplace(std::make_pair(compNameID, component));
    }

    const std::unordered_map<uint32_t, std::vector<uint8_t>>& NEntityFactory::ArchetypeHelper::GetArchetypeComponenetList(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
        return archetypeContainer.at(archetypeNameID).components;
    }

    const std::unordered_map<uint32_t, std::vector<uint8_t>>& NEntityFactory::ArchetypeHelper::GetArchetypeChildComponenetList(uint32_t archetypeNameID, uint32_t childNameID, const ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
       return  archetypeContainer.at(archetypeNameID).children.at(childNameID).components;
    }

    const ArchetypeContainer& NEntityFactory::ArchetypeHelper::GetChildren(uint32_t archetypeNameID, const ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
        return  archetypeContainer.at(archetypeNameID).children;
    }
}
