//------------------------------------------------------------------------------
//
// File Name:	NAnimationGraphManager.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "NAnimationGraphManager.h"
#include "imgui.h"
#include <nlohmann/json.hpp>

namespace NULLENGINE
{
    void NAnimationGraphManager::Load()
    {
        std::string folderPath = "../Assets/AnimationGraphs";

        if (!std::filesystem::exists(folderPath))
        {
            std::filesystem::create_directories(folderPath);
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".ag")
            {
                std::string fullPath = entry.path().string();
                std::string filename = entry.path().filename().string();
                size_t dotPos = filename.find_last_of('.');
                std::string name = (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;

                auto stateMachine = CreateRaw(name);
                stateMachine->LoadFromFile(fullPath); // Uses your existing LoadFromJson
            }
        }
    }

    void NAnimationGraphManager::SaveAllStateMachines()
    {
        std::string folderPath = "../Assets/AnimationGraphs";
        std::filesystem::create_directories(folderPath);

        for (const auto& [nameID, stateMachine] : m_ResourceList)
        {
            std::string filename = STRFROM(nameID) + ".ag";
            std::string filepath = folderPath + "/" + filename;
            stateMachine->SaveToFile(filepath); // StateMachine handles the file I/O
        }
    }

    void NAnimationGraphManager::Unload()
    {
    }

    //void NStateMachineManager::Unload()
    //{
    //    // TODO: implement unloading logic for StateMachine
    //}

}
