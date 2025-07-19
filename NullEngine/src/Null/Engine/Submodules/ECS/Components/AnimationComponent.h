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
		AnimationFlags_IsReversed = 1 << 4,
		// 3 bits left for future use
	};

	DEFINE_FLAG_SET(AnimationFlagSet,
		uint8_t m_IsRunning : 1;
		uint8_t m_IsLooping : 1;
		uint8_t m_IsPingPong : 1;
		uint8_t m_IsDone : 1;
		uint8_t m_IsReversed : 1;
		uint8_t m_Reserved : 3;
		);

	struct AnimationComponent
	{
		uint32_t       m_FrameIndex = 0;
		uint32_t       m_FrameCount = 0;
		uint32_t       m_FrameOffset = 0;
		float          m_FrameDelay = 0.0f;
		float          m_FrameDuration = 0.0f;

		ComponentFlagSet m_ComponentFlags;
		AnimationFlagSet m_Flags;
	};

}