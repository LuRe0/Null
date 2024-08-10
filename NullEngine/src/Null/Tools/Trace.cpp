
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
#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>
#include "spdlog/sinks/rotating_file_sink.h"
#include <sol/sol.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/args.h>
#include <spdlog/fmt/bundled/format.h>


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
	std::shared_ptr<ImguiSink_mt> Trace::s_ImguiSink;


	//std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> s_CoreFileSink;
	//std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> s_ClientFileSink;

	//void Trace::AddSink(void* sink)
	//{
	//	auto sink = dynamic_cast<ImguiSink_mt>
	//}
	fmt::dynamic_format_arg_store<fmt::format_context> InterpretArgs(sol::variadic_args& va)
	{
		fmt::dynamic_format_arg_store<fmt::format_context> ds;

		for (auto v : va)
		{
			if (v.is<int>()) {
				ds.push_back(v.as<int>());
			}
			else if (v.is<double>()) {
				ds.push_back(v.as<double>());
			}
			else if (v.is<std::string>()) {
				ds.push_back(v.as<std::string>());
			}
			else if (v.is<float>()) {
				ds.push_back(v.as<float>());
			}
			else {
				// Handle other types if needed
			}
		}


		return ds;
	}


	void Trace::trace(const char* format, sol::variadic_args va)
	{
		auto ds = InterpretArgs(va);

		std::string message = fmt::vformat(format, ds);

		NLE_TRACE(message);

	}

	void Trace::info(const char* format, sol::variadic_args va)
	{
		auto ds = InterpretArgs(va);

		std::string message = fmt::vformat(format, ds);

		NLE_INFO(message);
	}

	void Trace::warn(const char* format, sol::variadic_args va)
	{

		auto ds = InterpretArgs(va);
	
		std::string message = fmt::vformat(format, ds);

		NLE_WARN(message);
	}

	void Trace::debug(const char* format, sol::variadic_args va)
	{
		auto ds = InterpretArgs(va);

		std::string message = fmt::vformat(format, ds);

		NLE_DEBUG(message);
	}

	void Trace::error(const char* format, sol::variadic_args va)
	{
		auto ds = InterpretArgs(va);

		std::string message = fmt::vformat(format, ds);

		NLE_ERROR(message);
	}

	void Trace::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("NULL ENGINE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

		s_ImguiSink = std::make_shared<ImguiSink_mt>();

		s_ImguiSink.get()->set_pattern("%n: %v%$");

		auto s_CoreFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("../Assets/Log/Core/CoreLogger.txt", 1024 * 1024, 5, false);
		auto s_ClientFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("../Assets/Log/Client/ClientLogger.txt", 1024 * 1024, 5, false);

		s_CoreLogger.get()->sinks().push_back(s_ImguiSink);
		s_ClientLogger.get()->sinks().push_back(s_ImguiSink);
		s_ClientLogger.get()->sinks().push_back(s_ClientFileSink);

		//Trace::debug("DEEZ");
	}

	void Trace::RegisterToScripAPI(sol::state& lua)
	{

		lua.create_named_table("Trace",
			"trace", &Trace::trace,
			"info", &Trace::info,
			"warn", &Trace::warn,
			"debug", &Trace::debug,
			"error", &Trace::error
			// Register other static methods heresol::variadic_args va
		);
	}

}