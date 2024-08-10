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
#include "spdlog/sinks/rotating_file_sink.h"
#include "imgui.h"
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
	//class NLE_API EditorConsolePannel;

	template<typename Mutex>
	class ImguiSink : public spdlog::sinks::base_sink <Mutex>
	{
	public:
		//void SetParent(EditorConsolePannel* parent) { m_Parent = parent; }
		void print()
		{
			//std::cout << std::flush;
			for (const auto& str : m_Log)
			{
				switch (str.first)
				{
				case spdlog::level::trace:
					ImGui::TextColored({1,1,1,1},str.second.c_str());
					break;
				case spdlog::level::info:
					ImGui::TextColored({ 0,1,0,1 },str.second.c_str());
					break;
				case spdlog::level::warn:
					ImGui::TextColored({ 0.99,0.72,0.01,1 },str.second.c_str());
					break;
				case spdlog::level::err:
					ImGui::TextColored({ 1,0,0,1 },str.second.c_str());
					break;
				case spdlog::level::debug:
					ImGui::TextColored({ 0.01,0.82,0.98,1 },str.second.c_str());
					break;
				default:
					break;
				}
			}


			// Check if user is at the bottom
			float scrollY = ImGui::GetScrollY();
			float scrollMaxY = ImGui::GetScrollMaxY();
			float windowHeight = ImGui::GetWindowHeight();

			// Check if the cursor is at the very bottom of the scrollable area
			bool isAtBottom = scrollY >= (scrollMaxY-0.25f);

			// Auto-scroll only if the user is at the bottom of the scroll area
			if (isAtBottom)
			{
				ImGui::SetScrollHereY(1.0f); // Scroll to the bottom
			}

			//m_Log.clear();
		}
	private:
		//EditorConsolePannel* m_Parent;
		std::vector<std::pair<spdlog::level::level_enum, std::string>> m_Log;
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{

			// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
			// msg.raw contains pre formatted log

			// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

			m_Log.emplace_back(msg.level, fmt::to_string(formatted));

			if (m_Log.size() > 100) // Example limit
			{
				std::rotate(m_Log.begin(), m_Log.begin() + 10, m_Log.end());

				// Remove the last 10 elements which were originally the first 10
				m_Log.resize(m_Log.size() - 10);
			}
			//std::cout << ;
		}

	

		void flush_() override
		{
			//std::cout << std::flush;
			//for (const auto& str : m_Log)
			//{
			//	ImGui::Text(str.c_str);
			//}

			//m_Log.clear();
		}
	};

#include "spdlog/details/null_mutex.h"
#include <mutex>
	using ImguiSink_mt = ImguiSink<std::mutex>;
	using ImguiSink_st = ImguiSink<spdlog::details::null_mutex>;


}
