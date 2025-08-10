#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	enum AnimatorFlags : uint8_t
	{
		AnimatorFlags_HasEntered = 1 << 0,
		AnimatorFlags_ClipChanged = 1 << 1,
		AnimatorFlags_WaitingForEnd = 1 << 2,
		AnimatorFlags_Reserved = 1 << 3,
		// 3 bits left for future use
	};


	DEFINE_FLAG_SET(AnimatorFlagSet,
			uint8_t hasEntered : 1;          // Has entered current clip
			uint8_t clipChanged : 1;         // Clip changed this frame
			uint8_t waitingForEnd : 1;       // Waiting for current clip to finish
			uint8_t reserved : 5;
		);



    struct AnimatorComponent
    {
		uint32_t currentClipID = 0;
		uint32_t lastClipID = 0;

		// Runtime modifiers
		float playRate = 1.0f;

		AnimatorFlagSet flags;
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}