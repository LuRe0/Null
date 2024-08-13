
//------------------------------------------------------------------------------
//
// File Name:	CircleCollider2DSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "CircleCollider2DSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	CircleCollider2DSystem::CircleCollider2DSystem()
	{
		Require<TransformComponent>();
		Require<CircleCollider2DComponent>();
		//Require<CircleCollider2DComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<CircleCollider2DComponent>(CreateCircleCollider2DComponent,
			[this](Entity& id) { this->ViewCircleCollider2DComponent(id); }, WriteCircleCollider2DComponent);
	}

	void CircleCollider2DSystem::Load()
	{
	}

	void CircleCollider2DSystem::Init()
	{
		ISystem::Init();

	}

	void CircleCollider2DSystem::Update(float dt)
	{

	}

	void CircleCollider2DSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::ColorEdit4("Collider Color", glm::value_ptr(m_Color), 0.5f);
		ImGui::DragFloat("Collider Thickness", &m_Thickness, 0.5f);
	}

	void CircleCollider2DSystem::Render()
	{
		if (!NEngine::Instance().Get<NDebugManager>()->m_ShowColliders)
			return;

		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			CircleCollider2DComponent& cc2D = m_Parent->GetComponent<CircleCollider2DComponent>(entityId);

			if (!cc2D.m_RuntimeFixture)
				continue;



			auto translation = (transform.m_Translation + glm::vec3(cc2D.m_Offset, transform.m_Translation.z + transform.m_Scale.z + 0.50f));
			auto rot = cc2D.m_RuntimeFixture->GetBody()->GetAngle();

			
			glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * glm::vec4(translation, 1.0f);

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;

			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
			// Calculate rotation matrix (assuming Euler angles in radians)
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(glm::vec3(0, 0, rot))));

			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(cc2D.m_Radius, cc2D.m_Radius, 1.0f));

			glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;

			/*		matrix, meshManager->Get("Quad"), "", glm::vec4(0, 1, 0, 1), "",
						0, entityId, 0.05f, 0.005f, RenderData::INSTANCED)*/
						//model, mesh, spritesrc, tint, shadername, frameindex, entity
			renderer->AddRenderCall(std::make_unique<ElementData>(matrix, meshManager->Get("Circle"), nullptr, m_Color, "", 0,
				entityId, m_Thickness, 0.005f, RenderData::INSTANCED, -depth));
		}
	}

	void CircleCollider2DSystem::Unload()
	{
	}

	void CircleCollider2DSystem::Shutdown()
	{
	}

	void CircleCollider2DSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<CircleCollider2DComponent>
			(
				"BoxCollider2D",
				sol::no_constructor,
				"type_id", &Component<CircleCollider2DComponent>::GetID,
				"offset", sol::readonly(&CircleCollider2DComponent::m_Offset),
				"radius", sol::readonly(&CircleCollider2DComponent::m_Radius),
				"set_offset", sol::overload(
					[this](CircleCollider2DComponent& cc2d, float x, float y)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						cc2d.m_Offset = glm::vec2(x, y);

						if (cc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
							dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_p.Set(offset.x, offset.y);
						}
					},
					[this](CircleCollider2DComponent& cc2d, glm::vec2 newOffset)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						cc2d.m_Offset = newOffset;

						if (cc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
							dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_p.Set(offset.x, offset.y);
						}
					}
				),
				"set_radius", [this](CircleCollider2DComponent& cc2d, float r)
				{
					PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

					cc2d.m_Radius = r;

					if (cc2d.m_RuntimeFixture)
					{
						auto radius = physicsSys->PixelsToMeters(cc2d.m_Radius);

						dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_radius = radius;
					}
				}
		);
	}


	void CircleCollider2DSystem::CreateCircleCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<CircleCollider2DComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Offset = jsonWrapper.GetVec2("offset", { 0.0f, 0.0f });
			comp->m_Radius = jsonWrapper.GetFloat("radius", 50.0f);
			comp->m_Density = jsonWrapper.GetFloat("density", 1.0f);
			comp->m_Friction = jsonWrapper.GetFloat("friction", 0.5f);
			comp->m_Restitution = jsonWrapper.GetFloat("restitution", 1.0f);
			comp->m_RestitutionThreshold = jsonWrapper.GetFloat("restitutionThreshold", 0.5f);
		}
		componentFactory->AddOrUpdate<CircleCollider2DComponent>(id, comp, registry, comp->m_Offset, comp->m_Radius, comp->m_Density, comp->m_Friction, comp->m_Restitution, comp->m_RestitutionThreshold);

	}

	JSON CircleCollider2DSystem::WriteCircleCollider2DComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& collider = *static_cast<CircleCollider2DComponent*>(component);

		json["CircleCollider2D"]["offset"] = { collider.m_Offset.x, collider.m_Offset.y };
		json["CircleCollider2D"]["radius"] = collider.m_Radius;
		json["CircleCollider2D"]["density"] = collider.m_Density;
		json["CircleCollider2D"]["friction"] = collider.m_Friction;
		json["CircleCollider2D"]["restitution"] = collider.m_Restitution;
		json["CircleCollider2D"]["restitutionThreshold"] = collider.m_RestitutionThreshold;

		return json;

	}

	void CircleCollider2DSystem::ViewCircleCollider2DComponent(Entity& entity)
	{
		CircleCollider2DComponent& cc2d = entity.Get<CircleCollider2DComponent>();
		PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

		ImGui::DragFloat2("Offset", glm::value_ptr(cc2d.m_Offset), 0.5f);
		ImGui::DragFloat("radius", &cc2d.m_Radius, 0.5f);
		ImGui::DragFloat("Density", &cc2d.m_Density, 0.5f);
		ImGui::DragFloat("Friction", &cc2d.m_Friction, 0.5f, 0, 1.0f);
		ImGui::DragFloat("Resitution", &cc2d.m_Restitution, 0.5f);
		ImGui::DragFloat("Resitution Threshold", &cc2d.m_RestitutionThreshold, 0.5f);

		if (cc2d.m_RuntimeFixture)
		{

			cc2d.m_RuntimeFixture->SetDensity(cc2d.m_Density);
			cc2d.m_RuntimeFixture->SetFriction(cc2d.m_Friction);
			cc2d.m_RuntimeFixture->SetRestitution(cc2d.m_Restitution);
			cc2d.m_RuntimeFixture->SetRestitutionThreshold(cc2d.m_RestitutionThreshold);

			auto offset = physicsSys->PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
			auto radius = physicsSys->PixelsToMeters(cc2d.m_Radius);


			dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_p.Set(offset.x, offset.y);
			dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_radius = radius;
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires Rigidbody2D component to enable physics");
		}
	}
}