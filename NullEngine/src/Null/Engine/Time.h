#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Time.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "GLFW/glfw3.h"


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
	//! Wrapper class for delta time
	class NLE_API Time
	{
	public:

		// Get the delta time
		float DeltaTime() const {return m_deltaTime;}

		//! Initialize class
		static Time& Instance()
		{
			static Time instance;
			return instance;
		}

		//! Update current delta time
		void Update();

	private:
		// Private constructor and destructor
		Time() : m_previousTime(glfwGetTime()), m_deltaTime(0.0f) {}
		~Time() {}

		double m_previousTime;
		float m_deltaTime;

	};
}