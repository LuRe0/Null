
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ContentBrowserPannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include <shellapi.h>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	static const std::filesystem::path s_AssetsPath = "../Assets";

	bool IsImageFile(const std::string& extension) {
		static const std::set<std::string> imageExtensions = {
			".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tiff", ".ico", ".webp"
		};
		return imageExtensions.find(extension) != imageExtensions.end();
	}


	ContentBrowserPannel::ContentBrowserPannel() : m_CurrentDirectory(s_AssetsPath)
	{
	}

	void ContentBrowserPannel::OnImGUIRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != s_AssetsPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}

		}

		float cellSize = (m_ThumbnailSize + m_Padding) * m_Scale;

		float pannelWidth = ImGui::GetContentRegionAvail().x;

		int colCount = (int)(pannelWidth / cellSize);

		colCount = std::max(1, colCount);


		ImGui::PushItemWidth(100);
		static ImGuiTextFilter filter;
		ImGui::SameLine(ImGui::GetWindowWidth() - 450 - 50);
		ImGui::Text("Search:");
		ImGui::SameLine(ImGui::GetWindowWidth() - 350);
		filter.Draw("##searchbar", 340.f);
		ImGui::PopItemWidth();

		ImGui::Columns(colCount, 0, false);
		for (auto& p : std::filesystem::directory_iterator(m_CurrentDirectory))
		{

			const std::string& path = p.path().string();
			const std::string& stem = p.path().stem().string();
			const std::string& filename = p.path().filename().string();

			if (filter.PassFilter(filename.c_str()))
			{
				ImGui::PushID(filename.c_str());

				ImGui::Button(filename.c_str()) && ImGui::IsMouseDoubleClicked(0);

				std::filesystem::relative(p.path(), s_AssetsPath);

				std::string extension = p.path().extension().string();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (p.is_directory())
					{
						m_CurrentDirectory /= filename;
					}
					else
					{

						std::string fileP = std::filesystem::absolute(p.path()).string();
						ShellExecuteA(0, 0, fileP.c_str(), 0, 0, SW_SHOW);
					}
				}

				if (extension == ".ent")
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("ENTITY_FILE", stem.c_str(), stem.size() + 1);
						ImGui::Text("Dragging %s", filename.c_str());
						ImGui::EndDragDropSource();
					}
				}
				else if (extension == ".scene")
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("SCENE_FILE", stem.c_str(), stem.size() + 1);
						ImGui::Text("Dragging %s", filename.c_str());
						ImGui::EndDragDropSource();
					}
				}
				else if (IsImageFile(extension))
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("TEXTURE_FILE", stem.c_str(), stem.size() + 1);
						ImGui::Text("Dragging %s", filename.c_str());
						ImGui::EndDragDropSource();
					}
				}


				ImGui::PopID();

				ImGui::NextColumn();
			}

		}

		ImGui::Columns(1);
		ImGui::End();
	}
}
