
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
#include "../../Tools/JsonWrapper.h"
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
        std::string folderPath = "../Assets/Animations";

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
        NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();

        JsonReader jsonWrapper(json);

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
        clip.animationLength = json.value("animationLength", 0.1f);

        // Handle flags
        if (json.contains("flags"))
        {
            clip.flags.m_Flags = static_cast<uint8_t>(json.value("flags", 0));
        }

        glm::ivec2 dimension = jsonWrapper.GetVec2("dimension", { 1, 1 });
        auto src = jsonWrapper.GetString("spriteSheetID", "");
        if (!src.empty())
        {
            clip.spriteSheetID = STRID(src);
            spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
        }
    }


    void NAnimationClipManager::SaveClipToJson(const AnimationClip& clip, nlohmann::json& json, int row, int col)
    {

        json["name"] = STRFROM(clip.nameID);
        json["spriteSheetID"] = STRFROM(clip.spriteSheetID);
        json["startingFrame"] = clip.startingFrame;
        json["frameCount"] = clip.frameCount;
        json["animationLength"] = clip.animationLength;
        json["flags"] = clip.flags.m_Flags;

        auto* src = clip.spriteSheetID ? NSpriteSourceManager::Instance()->Get(clip.spriteSheetID) : nullptr;
        if (src)
        {
            json["dimension"] = nlohmann::json::array({ src->GetRows(), src->GetCols() });
        }
        else
        {
            json["dimension"] = nlohmann::json::array({ 1, 1 });
        }

        if (!Has(clip.nameID))
        {
			const std::string& name = STRFROM(clip.nameID);
			AnimationClip* newClip = CreateRaw(name);
			*newClip = clip; // Copy the clip data to the new clip
        }
        else
        {
            AnimationClip* existingClip = Get(clip.nameID);
            if (existingClip)
            {
                *existingClip = clip; // Update existing clip
            }
		}
    }

    void NAnimationClipManager::SaveClipToFile(const AnimationClip& clip, int row, int col)
    {
        std::string filePath = std::string("../Assets/Animations/") + STRFROM(clip.nameID) + std::string(".anim");

        nlohmann::json json;
        SaveClipToJson(clip, json);

        std::ofstream file(filePath);
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
            SaveClipToFile(*clip);
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