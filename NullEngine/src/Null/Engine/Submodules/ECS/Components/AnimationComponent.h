#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	//struct AnimationComponent : public BaseComponent
	//{
	//	unsigned int m_FrameIndex;
	//	unsigned int m_FrameCount;
	//	unsigned int m_FrameOffset; // Starting frame in the sprite sheet
	//	float m_FrameDelay;
	//	float m_FrameDuration;
	//	bool m_IsRunning;
	//	bool m_IsLooping;
	//	bool m_IsPingPong;
	//	bool m_IsDone;
	//	bool m_IsReversed;

	//	// Constructor with default values
	//	AnimationComponent(unsigned int frameCount = 0,
	//		unsigned int frameOffset = 0,
	//		float frameDuration = 0.0f,
	//		bool isLooping = false,
	//		bool isPingPong = false,
	//		bool isReversed = false,
	//		bool isRunning = false
	//	)
	//		: m_FrameIndex(0), // Always start at 0
	//		m_FrameCount(frameCount),
	//		m_FrameOffset(frameOffset),
	//		m_FrameDelay(0.0f),
	//		m_FrameDuration(frameDuration),
	//		m_IsRunning(isRunning),
	//		m_IsLooping(isLooping),
	//		m_IsPingPong(isPingPong),
	//		m_IsDone(false),
	//		m_IsReversed(isReversed)
	//	{}

	//	const std::string Name()const { return Component<AnimationComponent>::TypeName(); };
	//	const uint32_t ID() const { return Component<AnimationComponent>::GetID(); };

	//};


	enum AnimationFlags : uint8_t
	{
		AnimationFlags_IsRunning = 1 << 0,
		AnimationFlags_IsLooping = 1 << 1,
		AnimationFlags_IsPingPong = 1 << 2,
		AnimationFlags_IsDone = 1 << 3,
		AnimationFlags_PreserveFrame = 1 << 4,
		AnimationFlags_IsReversed = 1 << 5,
		// 3 bits left for future use
	};

	DEFINE_FLAG_SET(AnimationFlagSet,
		uint8_t isRunning : 1;
		uint8_t isLooping : 1;
		uint8_t isPingPong : 1;
		uint8_t isDone : 1;
		uint8_t isReversed : 1;
		uint8_t preserveFrame : 1;
		uint8_t reserved : 2;
		);

	struct AnimationComponent
	{
		uint32_t       frameIndex = 0;
		uint32_t       frameCount = 0;
		uint32_t       frameOffset = 0;
		float          frameDelay = 0.0f;
		float          frameDuration = 0.0f;

		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
		AnimationFlagSet flags = AnimationFlagSet(AnimationFlags_IsLooping);
	};

	struct EventPlacement {
		uint32_t eventAssetID = 0;  // 0 = unused slot
		uint32_t frame = 0;
		uint32_t endFrame = 0;      // if endFrame == frame -> instant event
		// if endFrame > frame  -> window event
	};

	struct AnimationClip
	{
		// notify events
		std::array<EventPlacement, 32> eventPlacements;
		uint32_t eventCount = 0;

		// Data that maps directly to AnimationComponent
		uint32_t nameID = 0;
		uint32_t spriteSheetID = 0;
		uint32_t startingFrame = 0;
		uint32_t frameCount = 1;
		float animationLength = 0.1f;
		float playRate = 1.0f; // Speed multiplier for playback
		AnimationFlagSet flags = AnimationFlagSet(AnimationFlags_IsLooping | AnimationFlags_PreserveFrame);
	};
}