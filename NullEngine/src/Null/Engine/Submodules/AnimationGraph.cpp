
//------------------------------------------------------------------------------
//
// File Name:	AnimationGraph.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "AnimationGraph.h"
#include "NIncludes.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    // StateMachine.cpp implementation
    const std::vector<uint32_t> AnimationGraph::s_emptyTransitions;

    AnimationGraph::AnimationGraph(const std::string& name)
    {
		m_nameID = STRID(name);
    }

    bool AnimationGraph::CanTransition(uint32_t fromClipID, uint32_t toClipID) const
    {
        // Bypass clips can interrupt anything
        if (m_bypassClips.contains(toClipID))
            return true;

        // Check if transition is allowed
        auto it = m_allowedTransitions.find(fromClipID);
        if (it != m_allowedTransitions.end())
        {
            const auto& transitions = it->second;
            return std::find(transitions.begin(), transitions.end(), toClipID) != transitions.end();
        }

        return false; // No transitions defined for this clip
    }

    void AnimationGraph::AddTransition(uint32_t fromClipID, uint32_t toClipID)
    {
        auto& transitions = m_allowedTransitions[fromClipID];
        if (std::find(transitions.begin(), transitions.end(), toClipID) == transitions.end())
        {
            transitions.push_back(toClipID);
        }
    }

    void AnimationGraph::RemoveTransition(uint32_t fromClipID, uint32_t toClipID)
    {
        auto it = m_allowedTransitions.find(fromClipID);
        if (it != m_allowedTransitions.end())
        {
            auto& transitions = it->second;
            transitions.erase(std::remove(transitions.begin(), transitions.end(), toClipID), transitions.end());

            // Remove empty entries
            if (transitions.empty())
            {
                m_allowedTransitions.erase(it);
            }
        }
    }

    void AnimationGraph::AddBypassClip(uint32_t clipID)
    {
        m_bypassClips.insert(clipID);
    }

    void AnimationGraph::RemoveBypassClip(uint32_t clipID)
    {
        m_bypassClips.erase(clipID);
    }

    const std::vector<uint32_t>& AnimationGraph::GetAllowedTransitions(uint32_t fromClipID) const
    {
        auto it = m_allowedTransitions.find(fromClipID);
        return (it != m_allowedTransitions.end()) ? it->second : s_emptyTransitions;
    }

    void AnimationGraph::LoadFromFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            // Log error or handle missing file
            return;
        }

        nlohmann::json json;
        file >> json;
        file.close();

        LoadFromJson(json); // Use your existing method
    }

    void AnimationGraph::LoadFromJson(const nlohmann::json& json)
    {
        if (json.contains("name"))
        {
            std::string name = json["name"];
            m_nameID = STRID(name);
        }

        if (json.contains("transitions"))
        {
            for (const auto& [fromClipName, toClipsArray] : json["transitions"].items())
            {
                uint32_t fromClipID = STRID(fromClipName);

                for (const auto& toClipName : toClipsArray)
                {
                    uint32_t toClipID = STRID(toClipName.get<std::string>());
                    AddTransition(fromClipID, toClipID);
                }
            }
        }

        if (json.contains("bypassClips"))
        {
            for (const auto& clipName : json["bypassClips"])
            {
                uint32_t clipID = STRID(clipName.get<std::string>());
                m_bypassClips.insert(clipID);
            }
        }
    }

    void AnimationGraph::SaveToFile(const std::string& filepath) const
    {
        nlohmann::json json = SaveToJson();

        std::ofstream file(filepath);
        if (file.is_open())
        {
            file << json.dump(4); // Pretty print with 4-space indent
            file.close();
        }
        else
        {
            // Optional: Log error about failed file write
			NLE_CORE_ERROR("Error opening file for writing: {0}", filepath);
        }
    }

    nlohmann::json AnimationGraph::SaveToJson() const
    {
        nlohmann::json json;
        json["name"] = STRFROM(m_nameID);

        // Save transitions with names
        for (const auto& [fromClipID, transitions] : m_allowedTransitions)
        {
            std::string fromClipName = STRFROM(fromClipID);
            std::vector<std::string> transitionNames;

            for (uint32_t toClipID : transitions)
            {
                transitionNames.push_back(STRFROM(toClipID));
            }

            json["transitions"][fromClipName] = transitionNames;
        }

        // Save bypass clips with names
        std::vector<std::string> bypassNames;
        for (uint32_t clipID : m_bypassClips)
        {
            bypassNames.push_back(STRFROM(clipID));
        }
        json["bypassClips"] = bypassNames;

        return json;
    }
}
