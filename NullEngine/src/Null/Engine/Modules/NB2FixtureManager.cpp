
//------------------------------------------------------------------------------
//
// File Name:	NB2FixtureManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NB2FixtureManager.h"
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
    //void NB2FixtureManager::Load()
    //{
    //}

    //void NB2FixtureManager::Init()
    //{
    //}

    //void NB2FixtureManager::Update(float dt)
    //{
    //}

    //void NB2FixtureManager::Unload()
    //{
    //    for (uint32_t i = 0; i < m_Resources.size(); ++i)
    //    {
    //        if (b2Fixture* fixture = m_Resources[i])
    //        {
    //            Handle<b2Fixture> handle{ i, m_Generations[i] };
    //            Destroy(handle); // central logic
    //        }
    //    }

    //    m_Resources.clear();
    //    m_Generations.clear();
    //    m_FreeIDs.clear();
    //    m_NameToID.clear();
    //}

    //void NB2FixtureManager::Destroy(const Handle<b2Fixture>& handle)
    //{
    //    if (handle.id >= m_Resources.size())
    //        return;
    //    if (m_Generations[handle.id] != handle.generation)
    //        return;

    //    b2Fixture* fixture = m_Resources[handle.id];
    //    if (fixture && fixture->GetBody())
    //    {
    //        fixture->GetBody()->DestroyFixture(fixture);
    //    }

    //    m_Resources[handle.id] = nullptr;
    //    m_Generations[handle.id]++;
    //    m_FreeIDs.push_back(handle.id);

    //    for (auto it_map = m_NameToID.begin(); it_map != m_NameToID.end(); )
    //    {
    //        if (it_map->second == handle.id)
    //            it_map = m_NameToID.erase(it_map);
    //        else
    //            ++it_map;
    //    }
    //}

    //void NB2FixtureManager::OnBodyDestroyed(b2Body* body)
    //{
    //    for (b2Fixture* f = body->GetFixtureList(); f != nullptr; f = f->GetNext())
    //    {
    //        // Linear search for fixture pointer in m_Resources vector
    //        auto it = std::find(m_Resources.begin(), m_Resources.end(), f);

    //        if (it != m_Resources.end())
    //        {
    //            size_t index = std::distance(m_Resources.begin(), it);


    //            m_Resources[index] = nullptr; 
    //            m_Generations[index]++;       
    //            m_FreeIDs.push_back(index);
    //    
    //            for (auto it_map = m_NameToID.begin(); it_map != m_NameToID.end(); )
    //            {
    //                if (it_map->second == index)
    //                    it_map = m_NameToID.erase(it_map);
    //                else
    //                    ++it_map;
    //            }
    //        }
    //    }
    //}



    //void NB2FixtureManager::Shutdown()
    //{
    //}
}
