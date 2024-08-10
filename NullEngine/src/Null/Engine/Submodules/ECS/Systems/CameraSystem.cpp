
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

		//NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &CameraSystem::OnWindowResize, eventManager);
	}

	void CameraSystem::Update(float dt)
	{
		//NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		//for (const auto entityId : GetSystemEntities())
		//{
		//	CameraComponent& Camera = m_Parent->GetComponent<CameraComponent>(entityId);

		//	if (!Camera.m_Enabled)
		//		continue;

		//	if (Camera.m_Dirty)
		//	{

		//		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), Camera.m_Translation);
		//		// Calculate rotation matrix (assuming Euler angles in radians)
		//		glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(Camera.m_Rotation)));

		//		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), Camera.m_Scale);

		//		Camera.m_CameraMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		//		Camera.m_Dirty = false;

		//		//if (m_Parent->HasComponent<Rigidbody2DComponent>(entityId))
		//		//{
		//		//	Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

		//		//	if (rb2d.m_RuntimeBody)
		//		//	{
		//		//		PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();
		//		//		auto pos = physicsSys->PixelsToMeters(Camera.m_Translation.x, Camera.m_Translation.y);
		//		//		rb2d.m_RuntimeBody->SetCamera({ pos.x, pos.y }, Camera.m_Rotation.z);
		//		//	}
		//		//}
		//	}
		//}
	}

	void CameraSystem::RuntimeUpdate(float dt)
	{
		//Update(dt);
	}


	void CameraSystem::Render()
	{
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
				"type_id", &Component<CameraComponent>::GetID
				//"translation", [](CameraComponent& Camera)
				//{ return std::make_tuple(Camera.m_Translation.x, Camera.m_Translation.y, Camera.m_Translation.z); },
				//"scale", [](CameraComponent& Camera)
				//{ return std::make_tuple(Camera.m_Scale.x, Camera.m_Scale.y, Camera.m_Scale.z); },
				//"rotation", [](CameraComponent& Camera)
				//{ return std::make_tuple(Camera.m_Rotation.x, Camera.m_Rotation.y, Camera.m_Rotation.z); },
				//"set_translation", [](CameraComponent& Camera, float x, float y, float z)
				//{
				//	Camera.m_Translation = glm::vec3(x, y, z);
				//	Camera.m_Dirty = Camera.m_DirectManipulation = true;
				//},
				//"set_scale", [](CameraComponent& Camera, float x, float y, float z)
				//{
				//	Camera.m_Scale = glm::vec3(x, y, z);
				//	Camera.m_Dirty = true;
				//},
				//"set_rotation", [](CameraComponent& Camera, float x, float y, float z)
				//{
				//	Camera.m_Scale = glm::vec3(x, y, z);
				//	Camera.m_Dirty = Camera.m_DirectManipulation = true;
				//}
		);
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
			comp->m_Camera = camManager->ReadCamera(json);
		}

		componentFactory->AddOrUpdate<CameraComponent>(id, comp, registry, comp->m_Name, comp->m_Type);
	}

	JSON CameraSystem::WriteCameraComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& cam = *static_cast<CameraComponent*>(component);

		cam.m_Camera->Write(json["Camera"]);

		return json;
	}

	void CameraSystem::ViewCameraComponent(Entity& entity)
	{
		CameraComponent& Camera = entity.Get<CameraComponent>();


		if (!Camera.m_Camera)
		{
			NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

			const auto& names3D = camManager->Get3DCameraNames();
			const auto& names2D = camManager->Get2DCameraNames();

			if (ImGui::BeginMenu("Add Camera"))
			{
				static ImGuiTextFilter filterBeh;


				filterBeh.Draw("##searchbarBehAdd");

				ImGui::Separator();

				ImGui::Text("Perspective");
				for (auto name : names3D)
				{
			
						if (filterBeh.PassFilter(name.c_str()))
						{
							if (ImGui::MenuItem(name.c_str()))
							{
							}
						}
				}

				ImGui::Text("Orthographic");
				for (auto name : names3D)
				{

						if (filterBeh.PassFilter(name.c_str()))
						{
							if (ImGui::MenuItem(name.c_str()))
							{
							}
						}
				}


				if (ImGui::MenuItem("New Camera"))
				{
					m_ShowCreationMenu = true;
				}


				if (m_ShowCreationMenu)
				{
					ImGui::OpenPopup("New Camera Name");
				}


				if (ImGui::BeginPopupModal("New Camera Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Enter the Camera name:");
					ImGui::InputText("##Cameraname", &m_CameraName);

					if (ImGui::Button("Create", ImVec2(120, 0)))
					{
						m_ShowCreationMenu = false; // Close the input box
						ImGui::CloseCurrentPopup();
			
						m_CameraName = "New Script";
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

				ImGui::EndMenu();

			}
		}
		else
		{
			Camera.m_Camera->View();
		}


		//if (ImGui::DragFloat3("Translation", glm::value_ptr(Camera.m_Translation), 0.5f))
		//{
		//	// Handle the change in translation
		//	//if (entity.Has<Rigidbody2DComponent>())
		//	//{
		//	//	Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();
		//	//	
		//	//	auto pos = physicsSys->PixelsToMeters(Camera.m_Translation.x, Camera.m_Translation.y);
		//	//	if(rb2d.m_RuntimeBody)
		//	//		rb2d.m_RuntimeBody->SetCamera({ pos.x, pos.y }, Camera.m_Rotation.z);
		//	//}

		//	Camera.m_Dirty = Camera.m_DirectManipulation = true;

		//}

		//if (ImGui::DragFloat3("Rotation", glm::value_ptr(Camera.m_Rotation), 0.5f))
		//{
		//	// Handle the change in rotation
		//	//if (entity.Has<Rigidbody2DComponent>())
		//	//{
		//	//	Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();

		//	//	auto pos = physicsSys->PixelsToMeters(Camera.m_Translation.x, Camera.m_Translation.y);
		//	//	if(rb2d.m_RuntimeBody)
		//	//		rb2d.m_RuntimeBody->SetCamera(rb2d.m_RuntimeBody->GetPosition(), Camera.m_Rotation.z);
		//	//}

		//	Camera.m_Dirty = Camera.m_DirectManipulation = true;

		//}

		//if (ImGui::DragFloat3("Scale", glm::value_ptr(Camera.m_Scale), 0.5f))
		//{
		//	// Handle the change in scale
		//	Camera.m_Dirty = true;
		//}

	}
}