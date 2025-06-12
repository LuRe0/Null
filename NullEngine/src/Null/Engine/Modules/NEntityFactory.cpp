
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
    }

    void NEntityFactory::Shutdown()
    {
    }
    void NEntityFactory::RegisterArchetype(const std::string& archetypeName)
    {


        ArchetypeHelper::AddArchetype(archetypeName, m_Archetypes);


    }

    Entity NEntityFactory::CreateEntity(const JSON& entityData, NRegistry* registry)
    {
        Entity newEntity(registry->CreateEntity(), registry);

        JsonReader jsonWrapper(entityData);

        if (!jsonWrapper.Empty())
        {

            NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

            const std::string& archetype = jsonWrapper.GetString("archetype", "");

            CloneOrCreateArchetype(archetype, newEntity, componentFactory, registry, entityData);
        }

        return newEntity;

    }

    Entity NEntityFactory::CreateEntity(NRegistry* registry)
    {
        return  Entity(registry->CreateEntity(), registry);;
    }

    void NEntityFactory::CloneOrCreateArchetype(const std::string& archetype, Entity& newEntity,
        NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData)
    {
        //if this is reading from
        if (!archetype.empty())
        {
            JsonReader jsonWrapper(entityData);

            newEntity.SetArchetype(archetype);

            if (!HasArchetype(archetype))
            {

                RegisterArchetype(archetype);

                ReadArchetype(archetype, newEntity, componentFactory, registry);

                if (newEntity.GetName().empty())
                {
                    const std::string& name = jsonWrapper.GetString("name", "");

                    if (!name.empty())
                        newEntity.SetName(name);
                    else
                        newEntity.SetName(archetype + " (Clone)");
                }

                return;
            }

            CloneComponents(componentFactory, archetype, registry, newEntity.GetID());

            CloneChild(newEntity, archetype, componentFactory, registry, JSON());

            if (newEntity.GetName().empty())
            {
                const std::string& name = jsonWrapper.GetString("name", "");

                if (!name.empty())
                    newEntity.SetName(name);
                else
                    newEntity.SetName(archetype + " (Clone)");
            }
        }
    }

    void NEntityFactory::CloneChild(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData)
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

        auto& children = ArchetypeHelper::GetChildren(parentArchetype, m_Archetypes);

        for (auto& child : children)
        {
            Entity childEntity = CreateEntity(registry);

            childEntity.SetName(child.first);
            childEntity.SetParentArchetype(parentArchetype);
            CloneChildComponents(componentFactory, parentArchetype, child.first, registry, childEntity.GetID(), m_Archetypes);

            registry->AddComponent<ParentComponent>(childEntity.GetID(), pEntity.GetID());
            auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(pEntity.GetID());
            childrenComponent.m_Children.push_back(childEntity.GetID());

            eventManager->TriggerEvent(EntityLoadedEvent(childEntity));

            CloneChild_Rec(childEntity, child.first, componentFactory, registry, entityData, children);
        }
    }

    void NEntityFactory::CloneChild_Rec(Entity& pEntity, const std::string& parentArchetype, NComponentFactory* componentFactory, NRegistry* registry, const JSON& entityData, ArchetypeContainer& archetypeDef)
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

        auto& children = ArchetypeHelper::GetChildren(parentArchetype, archetypeDef);

        for (auto& child : children)
        {
            Entity childEntity = CreateEntity(registry);

            childEntity.SetName(child.first);
            childEntity.SetParentArchetype(parentArchetype);
            CloneChildComponents(componentFactory, parentArchetype, child.first, registry, childEntity.GetID(), archetypeDef);

            registry->AddComponent<ParentComponent>(childEntity.GetID(), pEntity.GetID());
            auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(pEntity.GetID());
            childrenComponent.m_Children.push_back(childEntity.GetID());

            eventManager->TriggerEvent(EntityLoadedEvent(childEntity));

            CloneChild_Rec(childEntity, child.first, componentFactory, registry, entityData, children);
        }
    }



    void NEntityFactory::UpdateArchetype(const std::string& archetypeName, BaseComponent* component)
    {
       NLE_CORE_ASSERT(!ArchetypeHasComponent(archetypeName, component), "Component previously added to archetype!", archetypeName);
        
       ArchetypeHelper::UpdateArchetype(archetypeName, component, m_Archetypes);
    }

    void NEntityFactory::UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        ArchetypeHelper::UpdateArchetypeWithChild(archetypeName, childName, component, archetypeContainer);
    }

    bool NEntityFactory::HasArchetype(const std::string& archetypeName) const
    {
        return  ArchetypeHelper::HasArchetype(archetypeName, m_Archetypes);
    }
    
    bool NEntityFactory::ArchetypeHasComponent(const std::string& archetypeName, BaseComponent* component) const
    {
        return ArchetypeHelper::ArchetypeHasComponent(archetypeName, component, m_Archetypes);
    }

    void NEntityFactory::ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry, bool isChild)
    {
        std::string filePath = std::string("../Assets/Archetypes/") + filename + std::string(".ent");

        // Open the JSON file
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) {
            NLE_ERROR("Error: Could not open file");
            return;
        }

        // Parse the JSON file
        JSON entityData;

        try {
            inputFile >> entityData;
        }
        catch (JSON::parse_error& e) {
            NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
            return;
        }

        const std::string& archetypeName = filename;

        //auto components = entityData["components"];

        JsonReader archetypeWrapper(entityData);

        const std::string& name = archetypeWrapper.GetString("Name", "");

        if (entity.GetName().empty())
            entity.SetName(name);

        for (const auto& [componentName, componentData] : entityData["components"].items())
        {
            BaseComponent& component = componentFactory->CreateComponent(componentName + "Component", componentData, registry, entity.GetID());

       
            UpdateArchetype(archetypeName, &component);

            if (isChild)
                UpdateArchetypeWithChild(entity.m_ParentArchetype, entity.m_Name, &component, m_Archetypes);
        }


        if (entityData.contains("children"))
        {
            ReadChildrenFromArchetype(entity, archetypeName, entityData["children"], registry, componentFactory, m_Archetypes);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="parentEntity"></param>
    /// <param name="archetype"></param>
    /// <param name="childrenData"></param>
    /// <param name="registry"></param>
    /// <param name="componentFactory"></param>
    void NEntityFactory::ReadChildrenFromArchetype(Entity& parentEntity, const std::string& archetype, const nlohmann::json& childrenData, NRegistry* registry,
                                                  NComponentFactory* componentFactory, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

        for (const auto& childData : childrenData)
        {
            Entity childEntity = CreateEntity(childData, registry);
            const std::string& childName = childData.contains("name") ? childData["name"].get<std::string>() : "ChildEntity(" + std::to_string(childEntity.GetID()) + ")";
            std::string childArchetypeName = childName;
            childEntity.SetName(childName);
            childEntity.SetParentArchetype(parentEntity.m_Archetype);

            if (childData.contains(archetype))
            {
                JsonReader archetypeWrapper(childData);

                const std::string& archetype = archetypeWrapper.GetString("archetype", "");

                ReadArchetype(archetype, childEntity, componentFactory, registry);
                childArchetypeName = archetype;
            }


            if (childData.contains("components"))
            {
                for (const auto& [componentName, componentData] : childData["components"].items())
                {
                    BaseComponent& component = componentFactory->CreateComponent(componentName + "Component", componentData, registry, childEntity.GetID());
                    UpdateArchetypeWithChild(archetype, childName, &component, archetypeContainer);
                }
            }

            CloneChildComponents(componentFactory, archetype, childName, registry, childEntity.GetID(), archetypeContainer);

            registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
            auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
            childrenComponent.m_Children.push_back(childEntity.GetID());

            if (childData.contains("children"))
            {
                ReadChildrenFromArchetype(childEntity, childArchetypeName, childData["children"], registry, componentFactory, archetypeContainer[archetype].children);
            }

            eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));
            eventManager->TriggerEvent(EntityLoadedEvent(childEntity));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="componentFactory"></param>
    /// <param name="archetype"></param>
    /// <param name="registry"></param>
    /// <param name="id"></param>
    void NEntityFactory::CloneComponents(NComponentFactory* componentFactory, const std::string& archetype, NRegistry* registry, EntityID id)
    {
        std::unordered_map<std::string, BaseComponent*>& componentList = ArchetypeHelper::GetArchetypeComponenetList(archetype, m_Archetypes);

        for (auto component : componentList)
        {
            componentFactory->CloneComponent(component.first, component.second, JSON(), registry, id);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="componentFactory"></param>
    /// <param name="archetype"></param>
    /// <param name="childName"></param>
    /// <param name="registry"></param>
    /// <param name="id"></param>
    void NEntityFactory::CloneChildComponents(NComponentFactory* componentFactory, const std::string& archetype, const std::string& childName, NRegistry* registry, EntityID id, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        std::unordered_map<std::string, BaseComponent*>& componentList = ArchetypeHelper::GetArchetypeChildComponenetList(archetype,childName, archetypeContainer);

        for (auto component : componentList)
        {
            componentFactory->CloneComponent(component.first, component.second, JSON(), registry, id);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="lua"></param>
    void NEntityFactory::RegisterToScripAPI(sol::state& lua)
    {
        lua.new_usertype<Entity>("Entity",
            "get_component", [](Entity& entity, const sol::table& comp, sol::this_state s)
            {
                //const auto has_comp
                auto component = NComponentFactory::InvokeSolFunctions(NComponentFactory::GetIdType(comp), "get_component", entity, s);

                return component ? component.as<sol::reference>() : sol::lua_nil_t{};
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
                NSceneManager* sceneManager = NEngine::Instance().Get<NSceneManager>();

                sceneManager->GetCurrentScene()->DeleteEntity(entity.m_ID);
            },
        	"id", &Entity::m_ID,
        	"name", &Entity::m_Name
  
        );

    }

    NEntityFactory::ArchetypeHelper::ArchetypeHelper()
    {
    }

    NEntityFactory::ArchetypeHelper::~ArchetypeHelper()
    {
    }

    void NEntityFactory::ArchetypeHelper::AddArchetype(const std::string& archetype, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
		if (!archetypeContainer.contains(archetype))
		{
            EntityDefinition def;

            archetypeContainer.emplace(archetype, def);

			return;
		}

        NLE_CORE_INFO("Archetype {0} already exists!", archetype);
    }

    void NEntityFactory::ArchetypeHelper::AddArchetypeChild(const std::string& archetype, const std::string& child, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        if (archetypeContainer.contains(archetype))
        {
            if (!archetypeContainer[archetype].children.contains(child))
            {
                EntityDefinition def;
                archetypeContainer[archetype].children.emplace(child, def);
                return;
            }

            NLE_CORE_INFO("Child {0} already exists!", child);
        }

        NLE_CORE_INFO("Archetype {0} does not exists!", archetype);

    }

    bool NEntityFactory::ArchetypeHelper::HasArchetype(const std::string& archetype, const NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        return archetypeContainer.contains(archetype);
    }

    bool NEntityFactory::ArchetypeHelper::ArchetypeHasComponent(const std::string& archetype, BaseComponent* component, const NEntityFactory::ArchetypeContainer& archetypeContainer) 
    {
        const std::unordered_map<std::string, BaseComponent*>& componentList = archetypeContainer.at(archetype).components;

        return componentList.contains(component->Name());
    }

    bool NEntityFactory::ArchetypeHelper::ChildHasComponent(const std::string& archetype, const std::string& child, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        const std::unordered_map<std::string, BaseComponent*> componentList = archetypeContainer.at(archetype).children.at(child).components;

        return componentList.contains(component->Name());
    }

    void NEntityFactory::ArchetypeHelper::UpdateArchetype(const std::string& archetypeName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        archetypeContainer.at(archetypeName).components.emplace(std::make_pair(component->Name(), component));
    }

    void NEntityFactory::ArchetypeHelper::UpdateArchetypeWithChild(const std::string& archetypeName, const std::string& childName, BaseComponent* component, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        archetypeContainer[archetypeName].children[childName].components.emplace(std::make_pair(component->Name(), component));
    }

    std::unordered_map<std::string, BaseComponent*>& NEntityFactory::ArchetypeHelper::GetArchetypeComponenetList(const std::string& archetypeName, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
        return archetypeContainer[archetypeName].components;
    }

    std::unordered_map<std::string, BaseComponent*>& NEntityFactory::ArchetypeHelper::GetArchetypeChildComponenetList(const std::string& archetypeName, const std::string& childName, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
       return  archetypeContainer[archetypeName].children[childName].components;
    }

    NEntityFactory::ArchetypeContainer& NEntityFactory::ArchetypeHelper::GetChildren(const std::string& archetypeName, NEntityFactory::ArchetypeContainer& archetypeContainer)
    {
        // TODO: insert return statement here
        return  archetypeContainer[archetypeName].children;
    }
}
