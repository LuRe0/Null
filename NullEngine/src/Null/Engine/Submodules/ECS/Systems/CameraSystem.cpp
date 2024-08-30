
//------------------------------------------------------------------------------
//
// File Name:	CameraSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "CameraSystem.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	CameraSystem::CameraSystem()
	{
		Require<CameraComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<CameraComponent>(CreateCameraComponent,
			[this](Entity& id) { this->ViewCameraComponent(id); }, WriteCameraComponent);

	}


	void CameraSystem::Load()
	{
	}

	void CameraSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(EngineRunStateEvent, &CameraSystem::OnRuntimeStart, eventManager, EventPriority::Low);
	}

	void CameraSystem::Update(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		m_MainCameraCount = 0;

		for (const auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);

			if (!cam.m_Enabled)
				continue;

			cam.m_IsMainCamera ? ++m_MainCameraCount : m_MainCameraCount += 0;
		}
	}

	void CameraSystem::RuntimeUpdate(float dt)
	{
		//Update(dt);
	}


	void CameraSystem::Render()
	{
	}

	void CameraSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		if (m_MainCameraCount == 0)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Main Camera Found");
		}
		else if (m_MainCameraCount > 1)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Multiple Main Cameras Found. Please Only Set One");
		}
	}

	void CameraSystem::Unload()
	{
	}

	void CameraSystem::Shutdown()
	{
	}

	void CameraSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<CameraComponent>
			(
				"Camera",
				sol::no_constructor,
				"type_id", &Component<CameraComponent>::GetID,
				"get_position", [](CameraComponent& cam)
				{
					if (dynamic_cast<Camera3D*>(cam.m_Camera)) {
						auto camera = dynamic_cast<Camera3D*>(cam.m_Camera);
						return glm::vec2(camera->GetPosition().x, camera->GetPosition().y);
					}
					else
					{
						auto camera = dynamic_cast<Camera2D*>(cam.m_Camera);
						return camera->GetPosition();
					}
				},
				"set_position", 
				sol::overload([](CameraComponent& cam, float x, float y)
					{
						if (dynamic_cast<Camera3D*>(cam.m_Camera)) {
							auto camera = dynamic_cast<Camera3D*>(cam.m_Camera);
							camera->SetPosition(glm::vec3(x,y, camera->GetPosition().z));
						}
						else
						{
							auto camera = dynamic_cast<Camera2D*>(cam.m_Camera);
							camera->SetPosition(glm::vec2(x, y));
						}
					},
					[](CameraComponent& cam, glm::vec2 pos)
					{
						if (dynamic_cast<Camera3D*>(cam.m_Camera)) {
							auto camera = dynamic_cast<Camera3D*>(cam.m_Camera);
							camera->SetPosition(glm::vec3(pos, camera->GetPosition().z));
						}
						else
						{
							auto camera = dynamic_cast<Camera2D*>(cam.m_Camera);
							camera->SetPosition(pos);
						}
					}
				),

				"set_zoom", [](CameraComponent& cam, float z)
				{
					if (dynamic_cast<Camera3D*>(cam.m_Camera)) {
						auto camera = dynamic_cast<Camera3D*>(cam.m_Camera);
						camera->SetZoom(z);
					}
					else
					{
						auto camera = dynamic_cast<Camera2D*>(cam.m_Camera);
						camera->SetZoom(z);
					}
				});
	}


	bool CameraSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		for (const auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);

			if (!cam.m_Enabled)
				continue;

			if (!cam.m_IsMainCamera) 
			{
				continue;
			}
			else 
			{
				camManager->SetCurrentCamera(cam.m_Name);

				return true;
			}
		}

		return false;
	}

	void CameraSystem::CreateCameraComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		auto* comp = static_cast<CameraComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Name = jsonWrapper.GetString("name", "");
			comp->m_Type = jsonWrapper.GetString("type", "");
			comp->m_IsMainCamera = jsonWrapper.GetBool("mainCamera", false);
			comp->m_Camera = camManager->ReadCamera(json);
		}

		componentFactory->AddOrUpdate<CameraComponent>(id, comp, registry, comp->m_Name, comp->m_Type, comp->m_Camera, comp->m_IsMainCamera);
	}

	JSON CameraSystem::WriteCameraComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& cam = *static_cast<CameraComponent*>(component);
		json["Camera"]["mainCamera"] = cam.m_IsMainCamera;
		cam.m_Camera->Write(json["Camera"]);

		return json;
	}

	void CameraSystem::ViewCameraComponent(Entity& entity)
	{
		CameraComponent& Camera = entity.Get<CameraComponent>();


		if (!Camera.m_Camera)
		{
			NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();
			NWindow* window = m_Parent->Get<NWindow>();

			const auto& names3D = camManager->Get3DCameraNames();
			const auto& names2D = camManager->Get2DCameraNames();

			if (ImGui::BeginMenu("Add Camera"))
			{
				static ImGuiTextFilter filterBeh;


				filterBeh.Draw("##searchbarBehAdd");

				ImGui::Separator();

				ImGui::TextColored(ImVec4(0.3,0.4,1,1), "Perspective");
				for (auto name : names3D)
				{

					if (filterBeh.PassFilter(name.c_str()))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							Camera.m_Camera = camManager->GetCamera<Camera3D>(name);
						}
					}
				}

				ImGui::TextColored(ImVec4(0.3, 0.4, 1, 1), "Orthographic");

				for (auto name : names2D)
				{

					if (filterBeh.PassFilter(name.c_str()))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							Camera.m_Camera = camManager->GetCamera<Camera2D>(name);
						}
					}
				}


				if (ImGui::MenuItem("New Camera"))
				{
					m_ShowCreationMenu = true;
				}

				ImGui::EndMenu();

			}



			if (m_ShowCreationMenu)
			{
				ImGui::OpenPopup("New Camera Name");
			}



			if (ImGui::BeginPopupModal("New Camera Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Enter the Camera name:");
				ImGui::InputText("##Cameraname", &m_CameraName);

				ImGui::Text("Select Camera Projection:");
				if (ImGui::BeginCombo("##CameraProjection",
					magic_enum::enum_name(static_cast<Camera::CameraType>(m_CameraType)).data()))
				{
					for (size_t i = Camera::CameraType::ORTHOGRAPHIC; i < Camera::CameraType::PROJECTIONTYPE; i++)
					{
						auto name = magic_enum::enum_name(static_cast<Camera::CameraType>(i)).data();

						bool isSelected = m_CameraType == i;

						if (ImGui::Selectable(name, isSelected))
						{
							m_CameraType = static_cast<Camera::CameraType>(i);
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus(); // Set focus on the selected item
						}
					}

					ImGui::EndCombo();
				}


				if (ImGui::Button("Create", ImVec2(120, 0)))
				{

					if (m_CameraType == Camera::ORTHOGRAPHIC)
					{
						Camera.m_Camera = camManager->AddCamera<Camera2D>(m_CameraName, window->Width(), window->Height());
					}
					else if (m_CameraType == Camera::PERSPECTIVE)
					{
						Camera.m_Camera = camManager->AddCamera<Camera3D>(m_CameraName, window->Width(), window->Height());
					}

					Camera.m_Camera->SetName(m_CameraName);

					m_ShowCreationMenu = false; // Close the input box
					ImGui::CloseCurrentPopup();

					m_CameraName = "New Camera";
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					m_ShowCreationMenu = false; // Close the input box
					m_CameraName = "New Camera";
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

		}
		else
		{

			//ImGui::Text("Main Camera: ");
			//std::string checkboxLabel = "##Main Camera " + std::to_string(Camera.ID());
			ImGui::Checkbox("Main Camera", &Camera.m_IsMainCamera);
			Camera.m_Camera->View();
		}



	}
}