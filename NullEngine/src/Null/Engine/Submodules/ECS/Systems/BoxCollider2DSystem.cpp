
//------------------------------------------------------------------------------
//
// File Name:	BoxCollider2DSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "BoxCollider2DSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
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
	BoxCollider2DSystem::BoxCollider2DSystem()
	{
		Require<TransformComponent>();
		Require<BoxCollider2DComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<BoxCollider2DComponent>(CreateBoxCollider2DComponent,
			[this](Entity& id) { this->ViewBoxCollider2DComponent(id); }, WriteBoxCollider2DComponent);

	}

	void BoxCollider2DSystem::Load()
	{
	}

	void BoxCollider2DSystem::Init()
	{
		ISystem::Init();

	}

	void BoxCollider2DSystem::Update(float dt)
	{

	}

	void BoxCollider2DSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::ColorEdit4("Collider Color", glm::value_ptr(m_Color), 0.5f);
		ImGui::DragFloat("Collider Thickness", &m_Thickness, 0.5f);
	}

	void BoxCollider2DSystem::Render()
	{
		if (!NEngine::Instance().Get<NDebugManager>()->m_ShowDebug)
			return;

		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			BoxCollider2DComponent& bc2d = m_Parent->GetComponent<BoxCollider2DComponent>(entityId);

			if (!bc2d.m_RuntimeFixture)
				continue;

			auto translation = (transform.m_Translation + glm::vec3(bc2d.m_Offset, transform.m_Translation.z + transform.m_Scale.z*0.5f + 0.50f));
			auto rot = bc2d.m_RuntimeFixture->GetBody()->GetAngle();


			glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * glm::vec4(translation, 1.0f);

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;

			if (m_Parent->HasComponent<ParentComponent>(entityId))
			{
				auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

				TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);

				translation = (parentTransform.m_TransformMatrix * glm::vec4(translation, 1.0f));
			}

			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
			// Calculate rotation matrix (assuming Euler angles in radians)
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(glm::vec3(0,0, rot))));

			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(bc2d.m_Scale, 1.0f));

			glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;


	/*		matrix, meshManager->Get("Quad"), "", glm::vec4(0, 1, 0, 1), "",
				0, entityId, 0.05f, 0.005f, RenderData::INSTANCED)*/
			//model, mesh, spritesrc, tint, shadername, frameindex, entity
			renderer->AddDebugRenderCall(std::make_unique<ElementData>(matrix, meshManager->Get("Quad"), nullptr, m_Color, "", 0,
									entityId, m_Thickness, 0.005f, RenderData::INSTANCED, depth));
		}
	}

	void BoxCollider2DSystem::Unload()
	{
	}

	void BoxCollider2DSystem::Shutdown()
	{
	}

	void BoxCollider2DSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<BoxCollider2DComponent>
			(
				"BoxCollider2D",
				sol::no_constructor,
				"type_id", &Component<BoxCollider2DComponent>::GetID,
				"offset", sol::readonly(&BoxCollider2DComponent::m_Offset),
				"scale", sol::readonly(&BoxCollider2DComponent::m_Scale),
				"set_offset", sol::overload(
					[this](BoxCollider2DComponent& bc2d, float x, float y)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						bc2d.m_Offset = glm::vec2(x, y);

						if (bc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
							auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

							dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
						}
					},
					[this](BoxCollider2DComponent& bc2d, glm::vec2 newOffset)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						bc2d.m_Offset = newOffset;

						if (bc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
							auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

							dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
						}
					}
				),
				"set_scale", sol::overload(
					[this](BoxCollider2DComponent& bc2d, float x, float y)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						bc2d.m_Scale = glm::vec2(x, y);

						if (bc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
							auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

							dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
						}
					},
					[this](BoxCollider2DComponent& bc2d, glm::vec2 newScale)
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						bc2d.m_Scale = newScale;

						if (bc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
							auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

							dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
						}
					}
				)
		);
	}


	void BoxCollider2DSystem::CreateBoxCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<BoxCollider2DComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Offset = jsonWrapper.GetVec2("offset", { 0.0f, 0.0f });
			comp->m_Scale = jsonWrapper.GetVec2("scale", { 50.0f, 50.0f });
			comp->m_Density = jsonWrapper.GetFloat("density", 1.0f);
			comp->m_Friction = jsonWrapper.GetFloat("friction", 0.5f);
			comp->m_Restitution = jsonWrapper.GetFloat("restitution", 1.0f);
			comp->m_RestitutionThreshold = jsonWrapper.GetFloat("restitutionThreshold", 0.5f);
		}
		componentFactory->AddOrUpdate<BoxCollider2DComponent>(id, comp, registry, comp->m_Offset, comp->m_Scale, comp->m_Density, comp->m_Friction, comp->m_Restitution, comp->m_RestitutionThreshold);
	}

	JSON BoxCollider2DSystem::WriteBoxCollider2DComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& collider = *static_cast<BoxCollider2DComponent*>(component);

		json["BoxCollider2D"]["offset"] = { collider.m_Offset.x, collider.m_Offset.y };
		json["BoxCollider2D"]["scale"] = { collider.m_Scale.x, collider.m_Scale.y };
		json["BoxCollider2D"]["density"] = collider.m_Density;
		json["BoxCollider2D"]["friction"] = collider.m_Friction;
		json["BoxCollider2D"]["restitution"] = collider.m_Restitution;
		json["BoxCollider2D"]["restitutionThreshold"] = collider.m_RestitutionThreshold;

		return json;
	}

	void BoxCollider2DSystem::ViewBoxCollider2DComponent(Entity& entity)
	{
		BoxCollider2DComponent& bc2d = entity.Get<BoxCollider2DComponent>();
		PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();


		ImGui::DragFloat2("Offset", glm::value_ptr(bc2d.m_Offset), 0.5f);
		ImGui::DragFloat2("Scale", glm::value_ptr(bc2d.m_Scale), 0.5f);
		ImGui::DragFloat("Density", &bc2d.m_Density, 0.5f);
		ImGui::DragFloat("Friction", &bc2d.m_Friction, 0.5f, 0, 1.0f);
		ImGui::DragFloat("Resitution", &bc2d.m_Restitution, 0.5f);
		ImGui::DragFloat("Resitution Threshold", &bc2d.m_RestitutionThreshold, 0.5f);

		if (bc2d.m_RuntimeFixture)
		{

			bc2d.m_RuntimeFixture->SetDensity(bc2d.m_Density);
			bc2d.m_RuntimeFixture->SetFriction(bc2d.m_Friction);
			bc2d.m_RuntimeFixture->SetRestitution(bc2d.m_Restitution);
			bc2d.m_RuntimeFixture->SetRestitutionThreshold(bc2d.m_RestitutionThreshold);


			auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x/2, bc2d.m_Scale.y / 2);
			auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

			dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires Rigidbody2D component to enable physics");
		}
	}
}