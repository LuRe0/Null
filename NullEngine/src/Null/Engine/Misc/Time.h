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
		static float DeltaTime() {return m_DeltaTime;}

		//! Initialize class
		static Time& Instance()
		{
			static Time instance;
			return instance;
		}

		//! Update current delta time
		static void Update();

	private:
		// Private constructor and destructor
		Time()  {}
		~Time() {}

		static double m_PreviousTime;
		static float m_DeltaTime;

	};
}