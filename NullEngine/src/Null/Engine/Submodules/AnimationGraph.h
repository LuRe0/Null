#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	AnimationGraph.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

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
	class NLE_API AnimationGraph
	{
    public:
        AnimationGraph() = default;
        AnimationGraph(const std::string& name);
        ~AnimationGraph() = default;

        // Core functionality
        bool CanTransition(uint32_t fromClipID, uint32_t toClipID) const;
        void AddTransition(uint32_t fromClipID, uint32_t toClipID);
        void RemoveTransition(uint32_t fromClipID, uint32_t toClipID);
        void AddBypassClip(uint32_t clipID); // Clips that can interrupt anything
        void RemoveBypassClip(uint32_t clipID);

        // Serialization/Deserialization
        void LoadFromJson(const nlohmann::json& json);
        void SaveToFile(const std::string& filepath) const;
        nlohmann::json SaveToJson() const;

        // Getters
        const std::vector<uint32_t>& GetAllowedTransitions(uint32_t fromClipID) const;
        void LoadFromFile(const std::string& filepath);
        const std::unordered_set<uint32_t>& GetBypassClips() const { return m_bypassClips; }
        uint32_t GetNameID() const { return m_nameID; }
        void SetNameID(uint32_t nameID) { m_nameID = nameID; }

    private:
        uint32_t m_nameID = 0; // "HumanoidLocomotion", "EnemyAI", etc.

        // Transition rules: clipID -> list of valid next clips
        std::unordered_map<uint32_t, std::vector<uint32_t>> m_allowedTransitions;

        // Clips that can override any transition (death, stun, etc.)
        std::unordered_set<uint32_t> m_bypassClips;

        // Empty vector to return for clips with no transitions
        static const std::vector<uint32_t> s_emptyTransitions;
	};

}