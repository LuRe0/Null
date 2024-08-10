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
#include "ImguiSink.h"
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

		static void RegisterToScripAPI(sol::state& lua);


		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		inline static std::shared_ptr<ImguiSink_mt>& GetImguiSink() { return s_ImguiSink; }

		static Trace& Instance()
		{
			static Trace instance;
			return instance;
		}

	private:
		// Private constructor and destructor
		Trace() {}
		~Trace() {}

		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<ImguiSink_mt> s_ImguiSink;


		static void trace(const char* format, sol::variadic_args va);

		static void info(const char* format, sol::variadic_args va);

		static void warn(const char* format,  sol::variadic_args va);

		static void debug(const char* format, sol::variadic_args va);

		static void error(const char* format, sol::variadic_args va);

	};
}


#define NLE_CORE_TRACE(...)		::NULLENGINE::Trace::GetCoreLogger()->trace(__VA_ARGS__)
#define NLE_CORE_INFO(...)		::NULLENGINE::Trace::GetCoreLogger()->info(__VA_ARGS__)
#define NLE_CORE_WARN(...)		::NULLENGINE::Trace::GetCoreLogger()->warn(__VA_ARGS__)
#define NLE_CORE_DEBUG(...)		::NULLENGINE::Trace::GetCoreLogger()->debug(__VA_ARGS__)
#define NLE_CORE_ERROR(...)		::NULLENGINE::Trace::GetCoreLogger()->error(__VA_ARGS__)
//#define NLE_CORE_FATAL(...)		::NULLENGINE::Trace::GetCoreLogger()->fatal(__VA_ARGS__)
#define NLE_CORE_ASSERT(condition,...) \
    do { \
        if (!(condition)) { \
            NLE_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); \
            assert(false, __VA_ARGS_); \
        } \
    } while (0)




#define NLE_TRACE(...)		::NULLENGINE::Trace::GetClientLogger()->trace(__VA_ARGS__)
#define NLE_INFO(...)		::NULLENGINE::Trace::GetClientLogger()->info(__VA_ARGS__)
#define NLE_WARN(...)		::NULLENGINE::Trace::GetClientLogger()->warn(__VA_ARGS__)
#define NLE_ERROR(...)		::NULLENGINE::Trace::GetClientLogger()->error(__VA_ARGS__)
#define NLE_DEBUG(...)		::NULLENGINE::Trace::GetClientLogger()->debug(__VA_ARGS__)