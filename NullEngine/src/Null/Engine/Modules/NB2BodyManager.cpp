
//------------------------------------------------------------------------------
//
// File Name:	NB2BodyManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NB2BodyManager.h"
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
 //   void NB2BodyManager::Load()
 //   {
 //   }

 //   void NB2BodyManager::Init()
 //   {
 //   }

 //   void NB2BodyManager::Update(float dt)
 //   {
 //   }

	//void NB2BodyManager::Unload()
	//{
	//	for (uint32_t i = 0; i < m_Resources.size(); ++i)
	//	{
	//		b2Body* body = m_Resources[i];
	//		if (body)
	//		{
	//			// We need a world reference to destroy, so get it from the body
	//			b2World* world = body->GetWorld();
	//			Handle<b2Body> handle{ i, m_Generations[i] };
	//			Destroy(world, handle); // use central logic
	//		}
	//	}

	//	m_Resources.clear();
	//	m_Generations.clear();
	//	m_FreeIDs.clear();
	//	m_NameToID.clear();
	//}

	//void NB2BodyManager::Destroy(b2World* world, const Handle<b2Body>& handle)
	//{
	//	if (handle.id >= m_Resources.size())
	//		return;
	//	if (m_Generations[handle.id] != handle.generation)
	//		return;

	//	b2Body* body = m_Resources[handle.id];
	//	if (body)
	//	{
	//		const_cast<b2World*>(world)->DestroyBody(body); // Box2D requires non-const world
	//	}

	//	m_Resources[handle.id] = nullptr;
	//	m_Generations[handle.id]++;
	//	m_FreeIDs.push_back(handle.id);

	//	// Clean up any name mappings (if used)
	//	for (auto it = m_NameToID.begin(); it != m_NameToID.end(); )
	//	{
	//		if (it->second == handle.id)
	//			it = m_NameToID.erase(it);
	//		else
	//			++it;
	//	}
	//}

}
