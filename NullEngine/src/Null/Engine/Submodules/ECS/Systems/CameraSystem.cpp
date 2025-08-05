
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
#include "../../../../Tools/ImGuiH.h"
#include <NIncludes.h>
#include "../../../Submodules/Graphics/Camera/Camera2D.h"
#include "../../../Submodules/Graphics/Camera/Camera3D.h"




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
			[this](Entity& id) { this->ViewCameraComponent(id); }, WriteCameraComponent, AddCameraComponent, DiffCameraComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
		);

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

			if (!cam.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			cam.mainCamera ? ++m_MainCameraCount : m_MainCameraCount += 0;
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
					Camera* c =  NCameraManager::Instance()->GetCamera(cam.cameraID);
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
						Camera* c = NCameraManager::Instance()->GetCamera(cam.cameraID);

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
						Camera* c = NCameraManager::Instance()->GetCamera(cam.cameraID);

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
					Camera* c = NCameraManager::Instance()->GetCamera(cam.cameraID);

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
		ResetCameraStates();
		for (const auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);

			if (!cam.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			if (!cam.mainCamera)
			{
				continue;
			}
			else
			{
				camManager->SetCurrentCamera(STRFROM(cam.cameraID));

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
			comp->cameraID = STRID(jsonWrapper.GetString("name", ""));
			comp->typeID = STRID(jsonWrapper.GetString("type", ""));
			comp->mainCamera = jsonWrapper.GetBool("mainCamera", false);

			Camera* cam =  camManager->ReadCamera(json);

			if (auto* cam3D = dynamic_cast<Camera3D*>(cam))
			{
				comp->initData.position = jsonWrapper.GetVec3("position", glm::vec3(0.0f));
				comp->initData.fov = jsonWrapper.GetFloat("fov", 1.0f);
			}
			else if (auto* cam2D = dynamic_cast<Camera2D*>(cam))
			{
				comp->initData.position = glm::vec3(jsonWrapper.GetVec2("position", glm::vec2(0.0f)), 0.0f);
				comp->initData.zoom = jsonWrapper.GetFloat("zoom", 1.0f);
			}

		}

	}


	void CameraSystem::AddCameraComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<CameraComponent*>(component);
		componentFactory->AddOrUpdate<CameraComponent>(id, comp, registry,comp->initData, comp->cameraID, comp->typeID, comp->componentFlags, comp->mainCamera);
	}


	JSON CameraSystem::WriteCameraComponent(const void* component)
	{
		nlohmann::json json;

		auto& cam = *static_cast<const CameraComponent*>(component);

		if (!cam.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Camera"]["mainCamera"] = cam.mainCamera;
		json["Camera"]["ComponentFlags"] = cam.componentFlags.m_Flags;


		Camera* camera = NCameraManager::Instance()->GetCamera(cam.cameraID);
		if(camera)
			camera->Write(json["Camera"]);

		json["Camera"]["position"] = { cam.initData.position.x, cam.initData.position.y, cam.initData.position.z };
		json["Camera"]["zoom"] = cam.initData.zoom;
		json["Camera"]["fov"] = cam.initData.fov;

		return json;
	}

	JSON CameraSystem::DiffCameraComponent(const void* base, const void* modified)
	{
		auto* b = static_cast<const CameraComponent*>(modified);
		auto* a = static_cast<const CameraComponent*>(base);

		JSON diff;
		JSON cameraJson;

		// Compare main camera flag
		if (a->mainCamera != b->mainCamera)
			cameraJson["mainCamera"] = b->mainCamera;

		// Let the Camera objects write themselves into JSON for comparison
		JSON baseCamJson, modCamJson;
		Camera* cameraA = NCameraManager::Instance()->GetCamera(a->cameraID);
		Camera* cameraB = NCameraManager::Instance()->GetCamera(b->cameraID);


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

		if (a->componentFlags.m_Flags != b->componentFlags.m_Flags)
			diff["ComponentFlags"] = b->componentFlags.m_Flags;

		if (!cameraJson.empty())
			diff["Camera"] = cameraJson;

		return diff;
	}


	void CameraSystem::ViewCameraComponent(Entity& entity)
	{
		CameraComponent& Camera = entity.Get<CameraComponent>();

		NCameraManager* camManager = NCameraManager::Instance();

		NWindow* window = m_Parent->Get<NWindow>();

		CameraComponent& camComp = entity.Get<CameraComponent>();
		uint8_t& flags = camComp.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Camera", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<TransformComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

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
						Camera.cameraID = STRID(name);
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
						Camera.cameraID = STRID(name);
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
					if (auto* c = camManager->AddCamera<Camera2D>(STRID(m_CameraName), window->Width(), window->Height()))
					{
						Camera.cameraID = STRID(m_CameraName);
						c->SetName(m_CameraName);

					}
				}
				else if (m_CameraType == Camera::PERSPECTIVE)
				{
					if (auto* c = camManager->AddCamera<Camera3D>(STRID(m_CameraName), window->Width(), window->Height()))
					{
						Camera.cameraID = STRID(m_CameraName);
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

		auto* c = camManager->GetCamera(Camera.cameraID);

		if (c)
		{

			//ImGui::Text("Main Camera: ");
			//std::string checkboxLabel = "##Main Camera " + std::to_string(Camera.ID());
			ImGui::Checkbox("Main Camera", &Camera.mainCamera);
			if (Camera.mainCamera)
				if (ImGui::Checkbox("Preview", &m_Preview))
				{
					if (m_Preview)
						camManager->SetCurrentCamera(c->GetName());
					else
						camManager->SetCurrentCamera("Editor3D");
				}

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Initial Camera State");

			CameraInitData& init = Camera.initData;

			//ImGui::BeginDisabled(); // 🔒 Disable editing

			ImGui::InputFloat3("Init Position", glm::value_ptr(init.position));
			ImGui::InputFloat("Init Zoom", &init.zoom);
			ImGui::InputFloat("Init FOV", &init.fov);

			//ImGui::EndDisabled();

			c->View();
		}
		else
		{
			if (!entity.Has<TransformComponent>())
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Camera Selected!");
		}

		if (!enabled)
			ImGui::EndDisabled();




		ImGui::TreePop();
	}


	void CameraSystem::AddShake(const CameraShake& shake)
	{
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

	void CameraSystem::ResetCameraStates()
	{
		NRegistry* registry = NRegistry::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		for (auto entityId : GetSystemEntities())
		{
			CameraComponent& cam = registry->GetComponent<CameraComponent>(entityId);
			Camera* runtimeCam = camManager->GetCamera(cam.cameraID);

			if (!runtimeCam)
				continue;

			if (auto* c3D = dynamic_cast<Camera3D*>(runtimeCam))
			{
				c3D->SetPosition(cam.initData.position);
				c3D->SetFOV(cam.initData.fov);
			}
			else if (auto* c2D = dynamic_cast<Camera2D*>(runtimeCam))
			{
				c2D->SetPosition(glm::vec2(cam.initData.position));
				c2D->SetZoom(cam.initData.zoom);
			}
		}
	}

}