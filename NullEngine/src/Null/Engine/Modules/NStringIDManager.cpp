
//------------------------------------------------------------------------------
//
// File Name:	NStringIDManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NStringIDManager.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    NStringIDManager::NStringIDManager()
    {
        GetOrCreateID("");
    }
    void NStringIDManager::Load()
    {
    }

    void NStringIDManager::Init()
    {
    }

    void NStringIDManager::Update(float dt)
    {
    }

    void NStringIDManager::RuntimeUpdate(float dt)
    {
    }

    void NStringIDManager::Unload()
    {
    }

    void NStringIDManager::Shutdown()
    {
    }


    // Returns the ID of a string, creating it if necessary
    uint32_t NStringIDManager::GetOrCreateID(const std::string& str) 
    {
        auto it = m_StringToID.find(str);
        if (it != m_StringToID.end())
            return it->second;

        uint32_t id = m_NextID++;
        m_StringToID[str] = id;
        m_IDToString.push_back(str);
        return id;
    }

    // Returns ID if exists, otherwise returns invalid
    std::optional<uint32_t> NStringIDManager::TryGetID(const std::string& str) const 
    {
        auto it = m_StringToID.find(str);
        if (it != m_StringToID.end())
            return it->second;
        return std::nullopt;
    }

    // Returns string name for a given ID
    const std::string& NStringIDManager::GetString(uint32_t id) const 
    {
        NLE_CORE_ASSERT(id < m_IDToString.size(), "{} not found in list", id);
        return m_IDToString[id];
    }
    void NStringIDManager::RegisterToScripAPI(sol::state& lua)
    {
    }

    void NStringIDManager::RenderImGui()
    {
        //ImGui::Begin("StringID Manager");

        ImGui::Text("Total Strings: %zu", m_IDToString.size());

        static char filter[128] = "";
        ImGui::InputText("Filter", filter, sizeof(filter));

        ImGui::Separator();

        ImGui::BeginChild("StringList", ImVec2(0, 400), true);

        for (uint32_t id = 0; id < m_IDToString.size(); ++id)
        {
            const std::string& str = m_IDToString[id];

            if (filter[0] != '\0' && str.find(filter) == std::string::npos)
                continue;

            ImGui::Text("ID: %u | \"%s\"", id, str.c_str());
        }

        ImGui::EndChild();

        //ImGui::End();
    }

}
