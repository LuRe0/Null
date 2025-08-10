#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NAnimationClipManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Graphics/Texture.h"


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
	struct AnimationClip;

	// Specialize for resource
	class NLE_API NAnimationClipManager : public NResourceManager<AnimationClip, NAnimationClipManager>
	{
	public:


		void Load() override;

		void LoadClipFromJson(AnimationClip& clip, const nlohmann::json& json);

		void SaveClipToJson(const AnimationClip& clip, nlohmann::json& json, int row = 1, int col = 1);

		void SaveClipToFile(const AnimationClip& clip, int row = 1, int col = 1);

		void SaveAllClips();

		std::vector<AnimationClip*> GetClipsForSpriteSource(uint32_t spriteSourceID);

	};

}