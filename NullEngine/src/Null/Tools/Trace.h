#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Trace.h
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "spdlog/spdlog.h"
#include <memory>

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
namespace NULLENGINE
{
	class NLE_API Trace
	{
	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}


#define NLE_CORE_TRACE(...)		::NULLENGINE::Trace::GetCoreLogger()->trace(__VA_ARGS__)
#define NLE_CORE_INFO(...)		::NULLENGINE::Trace::GetCoreLogger()->info(__VA_ARGS__)
#define NLE_CORE_WARN(...)		::NULLENGINE::Trace::GetCoreLogger()->warn(__VA_ARGS__)
#define NLE_CORE_ERROR(...)		::NULLENGINE::Trace::GetCoreLogger()->error(__VA_ARGS__)
#define NLE_CORE_FATAL(...)		::NULLENGINE::Trace::GetCoreLogger()->fatal(__VA_ARGS__)




#define NLE_TRACE(...)		::NULLENGINE::Trace::GetClientLogger()->trace(__VA_ARGS__)
#define NLE_INFO(...)		::NULLENGINE::Trace::GetClientLogger()->info(__VA_ARGS__)
#define NLE_WARN(...)		::NULLENGINE::Trace::GetClientLogger()->warn(__VA_ARGS__)
#define NLE_ERROR(...)		::NULLENGINE::Trace::GetClientLogger()->error(__VA_ARGS__)
#define NLE_FATAL(...)		::NULLENGINE::Trace::GetClientLogger()->fatal(__VA_ARGS__)