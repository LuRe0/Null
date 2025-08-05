
//------------------------------------------------------------------------------
//
// File Name:	NAnimationClipManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NAnimationClipManager.h"
#include "Null/Engine/Submodules/ECS/Systems/PhysicsSystem.h"
#include "Null/Engine/Submodules/Graphics/Shader/Shader.h"
#include <NIncludes.h>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void NAnimationClipManager::Load()
    {
        std::string folderPath = "../Assets/animations";

        if (!std::filesystem::exists(folderPath))
        {
            std::filesystem::create_directories(folderPath);
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".anim")
            {
                std::string fullPath = entry.path().string();
                std::string filename = entry.path().filename().string();
                size_t dotPos = filename.find_last_of('.');
                std::string name = (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;

                // Read and parse JSON
                std::ifstream file(fullPath);
                if (!file.is_open()) continue;

                nlohmann::json json;
                file >> json;
                file.close();

                // Create clip and populate from JSON
                auto clip = CreateRaw(name);
                LoadClipFromJson(*clip, json);
            }
        }
    }

    void NAnimationClipManager::LoadClipFromJson(AnimationClip& clip, const nlohmann::json& json)
    {
        if (json.contains("name"))
        {
            std::string name = json["name"];
            clip.nameID = STRID(name);
        }

        if (json.contains("spriteSheetID"))
        {
            std::string spriteSheet = json["spriteSheetID"];
            clip.spriteSheetID = STRID(spriteSheet);
        }

        clip.startingFrame = json.value("startingFrame", 0);
        clip.frameCount = json.value("frameCount", 1);
        clip.frameDuration = json.value("frameDuration", 0.1f);

        // Handle flags
        if (json.contains("flags"))
        {
            clip.flags.m_Flags = 0;
            for (const auto& flagName : json["flags"])
            {
                std::string flag = flagName.get<std::string>();
                if (flag == "IsLooping") clip.flags.Set(AnimationFlags_IsLooping);
                if (flag == "PreserveFrame") clip.flags.Set(AnimationFlags_PreserveFrame);
            }
        }
    }


    void NAnimationClipManager::SaveClipToJson(const AnimationClip& clip, nlohmann::json& json)
    {
        json["name"] = STRFROM(clip.nameID);
        json["spriteSheetID"] = STRFROM(clip.spriteSheetID);
        json["startingFrame"] = clip.startingFrame;
        json["frameCount"] = clip.frameCount;
        json["frameDuration"] = clip.frameDuration;

        // Save flags as array of strings
        std::vector<std::string> flagNames;
        if (clip.flags.IsSet(AnimationFlags_IsLooping)) flagNames.push_back("IsLooping");
        if (clip.flags.IsSet(AnimationFlags_PreserveFrame)) flagNames.push_back("PreserveFrame");
        // Add other flags...

        json["flags"] = flagNames;
    }

    void NAnimationClipManager::SaveClipToFile(const AnimationClip& clip, const std::string& filepath)
    {
        nlohmann::json json;
        SaveClipToJson(clip, json);

        std::ofstream file(filepath);
        if (file.is_open())
        {
            file << json.dump(4); // Pretty print with 4-space indent
            file.close();
        }
    }

    void NAnimationClipManager::SaveAllClips()
    {
        std::string folderPath = "../Assets/animations";
        std::filesystem::create_directories(folderPath);

        for (const auto& [nameID, clip] : m_ResourceList)
        {
            std::string filename = STRFROM(nameID) + ".anim";
            std::string filepath = folderPath + "/" + filename;
            SaveClipToFile(*clip, filepath);
        }
    }

	std::vector<AnimationClip*> NAnimationClipManager::GetClipsForSpriteSource(uint32_t spriteSourceID)
	{
		std::vector<AnimationClip*> result;
		for (auto& [id, clip] : m_ResourceList) // or however you store clips
		{
			if (clip->spriteSheetID == spriteSourceID)
			{
				result.push_back(clip);
			}
		}
		return result;
	}
}