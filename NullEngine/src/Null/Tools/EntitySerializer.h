#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"

//#include "Entity.h"
//#include "Null/ECS/NRegistry.h"
//#include "Null/ECS/NEntityFactory.h"
//#include "Null/ECS/NComponentFactory.h"
//#include "Null/Event/NEventManager.h"
#include "Null/Tools/JsonWrapper.h"
//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


#pragma once


namespace NULLENGINE
{
	class Scene;
	class Entity;
	class NRegistry;
	class NEntityFactory;
	class NComponentFactory;
	class NEventManager;
	using EntityID = uint32_t;

	class EntitySerializer
	{
	public:

		static JSON SerializeEntity(const Entity& entity, NRegistry* registry, NComponentFactory* componentFactory);
		static JSON SerializeEntityWithChildren(const Entity& entity, NRegistry* registry, NComponentFactory* componentFactory, Scene* scene);

		static JSON SerializeComponents(EntityID id, NRegistry* registry, NComponentFactory* componentFactory);

		static Entity DeserializeEntity(const JSON& entityJson, NRegistry* registry,
			NEntityFactory* entityFactory, NComponentFactory* componentFactory,
			NEventManager* eventManager = nullptr, Scene* scene = nullptr);

		static Entity DeserializeEntityWithChildren(const JSON& entityJson, NRegistry* registry,
			NEntityFactory* entityFactory, NComponentFactory* componentFactory,
			NEventManager* eventManager = nullptr, Scene* scene = nullptr);
	};
}
