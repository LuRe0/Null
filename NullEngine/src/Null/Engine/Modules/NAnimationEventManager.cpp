//------------------------------------------------------------------------------
//
// File Name:	NAnimationEventManager.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "NAnimationEventManager.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../Tools/ScriptHelper.h"

namespace NULLENGINE
{

    void NAnimationEventManager::Load()
    {
        // TODO: implement loading logic for AnimationEvent
		std::string filePath = std::string("../Assets/AnimationEvents");

		for (const auto& entry : std::filesystem::directory_iterator(filePath))
		{
			if (entry.is_regular_file()) // Ensure it's a regular file (not a directory or symlink)
			{
				const std::string& filePath = entry.path().string();

				size_t lastSlash = filePath.find_last_of("\\/");
				size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

				size_t lastDot = filePath.find_last_of('.');
				size_t end = (lastDot == std::string::npos) ? filePath.length() : lastDot;

				const std::string& name = filePath.substr(start, end - start);
				auto extension = entry.path().extension().string();

				EventType type;
				if (extension == ".ae") {
					type = EventType::INSTANT;
				}
				else if (extension == ".aew") {
					type = EventType::WINDOW;
				}
				else {
					continue; // Skip files that aren't animation events
				}
				Create(name, filePath, type, *ScriptHelper::GetLuaState());
			}
		}
    }

    void NAnimationEventManager::Unload()
    {
        // TODO: implement unloading logic for AnimationEvent
    }

}
