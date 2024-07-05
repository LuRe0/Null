
//------------------------------------------------------------------------------
//
// File Name:	Trace.cpp
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Trace.h"
#include "spdlog/sinks/stdout_color_sinks.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	std::shared_ptr<spdlog::logger> Trace::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Trace::s_ClientLogger;

	void Trace::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("NULL ENGINE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

	}
}