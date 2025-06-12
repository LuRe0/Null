
//------------------------------------------------------------------------------
//
// File Name:	Time.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Time.h"
#include <algorithm>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	double Time::m_PreviousTime = glfwGetTime();
	float Time::m_DeltaTime = 0.0f;

	int Time::m_FrameCount = 0;
	int Time::m_FPS = 0;
	double Time::m_FPSLastTime = glfwGetTime();

	void Time::Update()
	{
		double currentTime = glfwGetTime();
		m_DeltaTime = static_cast<float>(currentTime - m_PreviousTime);
		m_PreviousTime = currentTime;
		m_DeltaTime = std::clamp(m_DeltaTime, 0.0f, 0.05f);


		// FPS counting
		m_FrameCount++;
		if (currentTime - m_FPSLastTime >= 1.0)
		{
			m_FPS = m_FrameCount;
			m_FrameCount = 0;
			m_FPSLastTime = currentTime;
		}
	}
}
