
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
#include "../Entities/Entity.h"




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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<CameraComponent>(CreateCameraComponent,
			[this](Entity& id) { this->ViewCameraComponent(id); }, WriteCameraComponent, AddCameraComponent, DiffCameraComponent);

	}


	void CameraSystem::Load()
	{
	}

	void CameraSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager =   NEventManager::Instance();


		SUBSCRIBE_EVENT(EngineRunStateEvent, &CameraSystem::OnRuntimeStart, eventManager, EventPriority::Low);
	}

	void CameraSystem::Update(float dt)
	{
		NRegistry* registry = NRegistry::Instance();

		m_MainCameraCount = 0;

		for (const auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);

			if (!cam.m_ComponentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			cam.m_IsMainCamera ? ++m_MainCameraCount : m_MainCameraCount += 0;
		}
	}

	void CameraSystem::RuntimeUpdate(float dt)
	{
		//Update(dt);
		UpdateCameraShake(dt);
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
					Camera* c =  NCameraManager::Instance()->GetCamera(cam.m_CameraID);
					if (dynamic_cast<Camera3D*>(c)) {
						auto camera = dynamic_cast<Camera3D*>(c);
						return glm::vec2(camera->GetPosition().x, camera->GetPosition().y);
					}
					else
					{
						auto camera = dynamic_cast<Camera2D*>(c);
						return camera->GetPosition();
					}
				},
				"set_position",
				sol::overload([](CameraComponent& cam, float x, float y)
					{
						Camera* c = NCameraManager::Instance()->GetCamera(cam.m_CameraID);

						if (dynamic_cast<Camera3D*>(c)) {
							auto camera = dynamic_cast<Camera3D*>(c);
							camera->SetPosition(glm::vec3(x, y, camera->GetPosition().z));
						}
						else
						{
							auto camera = dynamic_cast<Camera2D*>(c);
							camera->SetPosition(glm::vec2(x, y));
						}
					},
					[](CameraComponent& cam, glm::vec2 pos)
					{
						Camera* c = NCameraManager::Instance()->GetCamera(cam.m_CameraID);

						if (dynamic_cast<Camera3D*>(c)) {
							auto camera = dynamic_cast<Camera3D*>(c);
							camera->SetPosition(glm::vec3(pos, camera->GetPosition().z));
						}
						else
						{
							auto camera = dynamic_cast<Camera2D*>(c);
							camera->SetPosition(pos);
						}
					}
				),

				"add_shake", [](CameraComponent& cam, const CameraShake& shake) {
					CameraSystem* cameraSystem = NEngine::Instance().Get<CameraSystem>();
					if (cameraSystem) {
						cameraSystem->AddShake(shake);
					}
				},

				"set_zoom", [](CameraComponent& cam, float z)
				{
					Camera* c = NCameraManager::Instance()->GetCamera(cam.m_CameraID);

					if (dynamic_cast<Camera3D*>(c)) {
						auto camera = dynamic_cast<Camera3D*>(c);
						camera->SetZoom(z);
					}
					else
					{
						auto camera = dynamic_cast<Camera2D*>(c);
						camera->SetZoom(z);
					}
				});


		lua.new_usertype<CameraShake>(
			"CameraShake",
			sol::constructors<
			CameraShake(),
			CameraShake(float, float, float, EasingType, glm::vec2, float)>(), // if you want a param constructor
			"duration", &CameraShake::duration,
			"elapsed", &CameraShake::elapsed,
			"magnitude", &CameraShake::magnitude,
			"easing", &CameraShake::easing,
			"direction", &CameraShake::direction,
			"frequency", &CameraShake::frequency,
			"finished", &CameraShake::finished
		);
	}


	bool CameraSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		for (const auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);

			if (!cam.m_ComponentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			if (!cam.m_IsMainCamera)
			{
				continue;
			}
			else
			{
				camManager->SetCurrentCamera(STRFROM(cam.m_CameraID));

				return true;
			}
		}

		return false;
	}

	void CameraSystem::CreateCameraComponent(void* component, const nlohmann::json& json)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		auto* comp = static_cast<CameraComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_CameraID = STRID(jsonWrapper.GetString("name", ""));
			comp->m_TypeID = STRID(jsonWrapper.GetString("type", ""));
			comp->m_IsMainCamera = jsonWrapper.GetBool("mainCamera", false);

			camManager->ReadCamera(json);
		}

	}


	void CameraSystem::AddCameraComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<CameraComponent*>(component);
		componentFactory->AddOrUpdate<CameraComponent>(id, comp, registry, comp->m_CameraID, comp->m_TypeID, comp->m_ComponentFlags, comp->m_IsMainCamera);
	}


	JSON CameraSystem::WriteCameraComponent(const void* component)
	{
		nlohmann::json json;

		auto& cam = *static_cast<const CameraComponent*>(component);
		json["Camera"]["mainCamera"] = cam.m_IsMainCamera;
		json["Camera"]["ComponentFlags"] = cam.m_ComponentFlags.m_Flags;

		Camera* camera = NCameraManager::Instance()->GetCamera(cam.m_CameraID);
		if(camera)
			camera->Write(json["Camera"]);

		return json;
	}

	JSON CameraSystem::DiffCameraComponent(const void* base, const void* modified)
	{
		auto* b = static_cast<const CameraComponent*>(modified);
		auto* a = static_cast<const CameraComponent*>(base);

		JSON diff;
		JSON cameraJson;

		// Compare main camera flag
		if (a->m_IsMainCamera != b->m_IsMainCamera)
			cameraJson["mainCamera"] = b->m_IsMainCamera;

		// Let the Camera objects write themselves into JSON for comparison
		JSON baseCamJson, modCamJson;
		Camera* cameraA = NCameraManager::Instance()->GetCamera(a->m_CameraID);
		Camera* cameraB = NCameraManager::Instance()->GetCamera(b->m_CameraID);


		if (cameraA) cameraA->Write(baseCamJson);
		if (cameraB) cameraB->Write(modCamJson);

		// Compare each field individually in the written JSON
		for (auto& [key, modValue] : modCamJson.items())
		{
			if (!baseCamJson.contains(key) || baseCamJson[key] != modValue)
			{
				cameraJson[key] = modValue;
			}
		}

		if (a->m_ComponentFlags.m_Flags != b->m_ComponentFlags.m_Flags)
			diff["ComponentFlags"] = b->m_ComponentFlags.m_Flags;

		if (!cameraJson.empty())
			diff["Camera"] = cameraJson;

		return diff;
	}


	void CameraSystem::ViewCameraComponent(Entity& entity)
	{
		CameraComponent& Camera = entity.Get<CameraComponent>();

		NCameraManager* camManager = NCameraManager::Instance();

		NWindow* window = m_Parent->Get<NWindow>();

		const auto& names3D = camManager->Get3DCameraNames();
		const auto& names2D = camManager->Get2DCameraNames();

		if (ImGui::BeginMenu("Add Camera"))
		{
			static ImGuiTextFilter filterBeh;


			filterBeh.Draw("##searchbarBehAdd");

			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.3f, 0.4f, 1.0f, 1.0f), "Perspective");
			for (auto name : names3D)
			{

				if (filterBeh.PassFilter(name.c_str()))
				{
					if (ImGui::MenuItem(name.c_str()))
					{
						Camera.m_CameraID = STRID(name);
					}
				}
			}

			ImGui::TextColored(ImVec4(0.3f, 0.4f, 1.0f, 1.0f), "Orthographic");

			for (auto name : names2D)
			{

				if (filterBeh.PassFilter(name.c_str()))
				{
					if (ImGui::MenuItem(name.c_str()))
					{
						Camera.m_CameraID = STRID(name);
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


			if (ImGui::Button("Create", ImVec2(120.0f, 0.0f)))
			{

				if (m_CameraType == Camera::ORTHOGRAPHIC)
				{
					if (auto* c =camManager->AddCamera<Camera2D>(m_CameraName, window->Width(), window->Height()))
					{
						Camera.m_CameraID = STRID(m_CameraName);
						c->SetName(m_CameraName);

					}
				}
				else if (m_CameraType == Camera::PERSPECTIVE)
				{
					if (auto* c = camManager->AddCamera<Camera3D>(m_CameraName, window->Width(), window->Height()))
					{
						Camera.m_CameraID = STRID(m_CameraName);
						c->SetName(m_CameraName);
					}
				}


				m_ShowCreationMenu = false; // Close the input box
				ImGui::CloseCurrentPopup();

				m_CameraName = "New Camera";
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
			{
				m_ShowCreationMenu = false; // Close the input box
				m_CameraName = "New Camera";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		auto* c = camManager->GetCamera(Camera.m_CameraID);

		if (c)
		{

			//ImGui::Text("Main Camera: ");
			//std::string checkboxLabel = "##Main Camera " + std::to_string(Camera.ID());
			ImGui::Checkbox("Main Camera", &Camera.m_IsMainCamera);
			if (Camera.m_IsMainCamera)
				if (ImGui::Checkbox("Preview", &m_Preview))
				{
					if (m_Preview)
						camManager->SetCurrentCamera(c->GetName());
					else
						camManager->SetCurrentCamera("Editor3D");
				}

			c->View();
		}
		else
		{
			if (!entity.Has<TransformComponent>())
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Camera Selected!");
		}


	}


	void CameraSystem::AddShake(const CameraShake& shake)
	{
		m_ActiveShakes.emplace_back(shake);
	}


	void CameraSystem::UpdateCameraShake(float dt)
	{
		glm::vec2 totalOffset = {};

		for (auto it = m_ActiveShakes.begin(); it != m_ActiveShakes.end();)
		{
			glm::vec2 offset = it->GetOffset(dt);
			totalOffset += offset;

			if (it->IsFinished())
				it = m_ActiveShakes.erase(it);
			else
				++it;
		}

		NCameraManager* camManager = NCameraManager::Instance();

		if (auto cam = camManager->GetCurrentCamera())
		{
			cam->SetShakeOffset(glm::vec3(totalOffset, 0.0f)); // or whatever applies it
		}
	}
}