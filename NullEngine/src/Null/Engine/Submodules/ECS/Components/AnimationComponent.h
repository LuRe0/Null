#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	struct AnimationComponent : public BaseComponent
	{
		unsigned int m_FrameIndex;
		unsigned int m_FrameCount;
		unsigned int m_FrameOffset; // Starting frame in the sprite sheet
		float m_FrameDelay;
		float m_FrameDuration;
		bool m_IsRunning;
		bool m_IsLooping;
		bool m_IsPingPong;
		bool m_IsDone;
		bool m_IsReversed;

		// Constructor with default values
		AnimationComponent(unsigned int frameCount = 0,
			unsigned int frameOffset = 0,
			float frameDuration = 0.0f,
			bool isLooping = false,
			bool isPingPong = false,
			bool isReversed = false,
			bool isRunning = false
		)
			: m_FrameIndex(0), // Always start at 0
			m_FrameCount(frameCount),
			m_FrameOffset(frameOffset),
			m_FrameDelay(0.0f),
			m_FrameDuration(frameDuration),
			m_IsRunning(isRunning),
			m_IsLooping(isLooping),
			m_IsPingPong(isPingPong),
			m_IsDone(false),
			m_IsReversed(isReversed)
		{}

		const std::string Name() { return Component<AnimationComponent>::TypeName(); };
	};

}