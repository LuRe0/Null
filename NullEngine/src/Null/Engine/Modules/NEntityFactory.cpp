
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
        if (!m_Archetypes.contains(archetypeName))
        {
            m_Archetypes.emplace(std::make_pair(archetypeName, std::unordered_map<std::string,BaseComponent*>()));

            return;
        }

        NLE_CORE_INFO("Archetype {0} already exists!", archetypeName);
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



    void NEntityFactory::UpdateArchetype(const std::string& archetypeName, BaseComponent* component)
    {
        NLE_CORE_ASSERT(!ArchetypeHasComponent(archetypeName, component), "Component previously added to archetype!", archetypeName);

        m_Archetypes.at(archetypeName).emplace(std::make_pair(component->Name(), component));
    }

    bool NEntityFactory::HasArchetype(const std::string& archetypeName) const
    {
        return  m_Archetypes.contains(archetypeName);
    }

    bool NEntityFactory::ArchetypeHasComponent(const std::string& archetypeName, BaseComponent* component) const
    {
        std::unordered_map<std::string, BaseComponent*> componentList = m_Archetypes.at(archetypeName);

        return componentList.contains(component->Name());
    }

    void NEntityFactory::ReadArchetype(const std::string& filename, Entity& entity, NComponentFactory* componentFactory, NRegistry* registry)
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

        for (const auto& [componentName, componentData] : entityData["components"].items())
        {
            BaseComponent& component = componentFactory->CreateComponent(componentName + "Component", componentData, registry, entity.GetID());

            UpdateArchetype(archetypeName, &component);
        }


        JsonReader archetypeWrapper(entityData);

       const std::string& name = archetypeWrapper.GetString("Name", "");

       if(entity.GetName().empty())
            entity.SetName(name);
    }

    void NEntityFactory::CloneComponents(NComponentFactory* componentFactory, const std::string& archetype, NRegistry* registry, EntityID id)
    {
        std::unordered_map<std::string, BaseComponent*>& componentList = m_Archetypes.at(archetype);

        for (auto component : componentList)
        {
            componentFactory->CloneComponent(component.first, component.second, JSON(), registry, id);
        }
    }
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
}
