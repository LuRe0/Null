
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
#include <box2d/box2d.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../Entities/Entity.h"
#include "../../Scene.h"
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"
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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<BoxCollider2DComponent>(CreateBoxCollider2DComponent,
			[this](Entity& id) { this->ViewBoxCollider2DComponent(id); },
			WriteBoxCollider2DComponent, AddBoxCollider2DComponent, DiffBoxCollider2DComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
		);

	}

	void BoxCollider2DSystem::Load()
	{
	}

	void BoxCollider2DSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(EntityCreatedEvent, &BoxCollider2DSystem::OnEntityCreated, eventManager, EventPriority::Low);
		//SUBSCRIBE_EVENT(EntityDestroyedEvent, &BoxCollider2DSystem::OnEntityDestroyed, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &BoxCollider2DSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &BoxCollider2DSystem::OnEntityComponentAdded, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(SceneSwitchEvent, &BoxCollider2DSystem::OnSceneSwitched, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(InitializeBox2DColliderEvent, &BoxCollider2DSystem::OnInitializeBox2DStart, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntityParentedEvent, &PhysicsSystem::OnEntityParented, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntitySeparatedEvent, &PhysicsSystem::OnEntitySeparated, eventManager, EventPriority::High);

		NRegistry* registry = NRegistry::Instance();


		for (const auto entityId : GetSystemEntities())
		{
			eventManager->QueueEvent(std::make_unique<InitializeBox2DColliderEvent>(entityId));
		}
	}

	void BoxCollider2DSystem::Update(float dt)
	{

	}

	void BoxCollider2DSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::ColorEdit4("Collider Color", glm::value_ptr(m_Color));
		ImGui::DragFloat("Collider Thickness", &m_Thickness, 0.5f);
	}

	void BoxCollider2DSystem::Render()
	{
		if (!NDebugManager::Instance()->m_ShowDebug)
			return;

		NRenderer* renderer = NRenderer::Instance();
		NRegistry* m_Parent = NRegistry::Instance();
		NMeshManager* meshManager = NMeshManager::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			BoxCollider2DComponent& bc2d = m_Parent->GetComponent<BoxCollider2DComponent>(entityId);


			if (!bc2d.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			b2Body* body = PhysicsSystem::Instance()->GetActiveBody(bc2d.runtimeBodyIndex);

			if (!body)
				continue;



			glm::vec3 translation = (transform.translation);
			auto rot = body->GetAngle();



			for (size_t i = 0; i < bc2d.colliderCount; i++)
			{
				const BoxCollider2D& collider = bc2d.colliders[i];

				b2Fixture* fixture = PhysicsSystem::Instance()->GetActiveFixture(collider.runtimeFixtureIndex);

				


				auto translation = camManager->GetCurrentCamera()->GetCameraType() == Camera::PERSPECTIVE ?
					(transform.translation + glm::vec3(collider.offset, transform.translation.z + transform.scale.z + 0.50f)) :
					(transform.translation + glm::vec3(collider.offset, transform.translation.z + 0.50f));

				auto rot = body->GetAngle();
				//transform

				glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();

				// Transform the world position to camera space
				glm::vec4 cameraSpacePosition = viewMatrix * glm::vec4(translation, 1.0f);

				// The depth is the z-component of the camera space position
				float depth = cameraSpacePosition.z;


				if (m_Parent->HasComponent<ParentComponent>(entityId))
				{
					auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

					TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);

					translation = (parentTransform.transformMatrix * glm::vec4(translation, 1.0f));
				}

				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
				// Calculate rotation matrix (assuming Euler angles in radians)
				glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(glm::vec3(0, 0, rot))));

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(collider.scale, 1.0f));

				glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;


				/*		matrix, meshManager->Get("Quad"), "", glm::vec4(0, 1, 0, 1), "",
							0, entityId, 0.05f, 0.005f, RenderData::INSTANCED)*/
							//model, mesh, spritesrc, tint, shadername, frameindex, entity
				renderer->AddRenderCall(RenderCommandTypes::Debug, std::make_unique<ElementData>(matrix, meshManager->Get("Quad"), nullptr, m_Color, "", 0,
					entityId, m_Thickness, 0.005f, RenderData::INSTANCED, depth));

			}
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
	/*	lua.new_usertype<BoxCollider2DComponent>
			(
				"BoxCollider2D",
				sol::no_constructor,
				"type_id", &Component<BoxCollider2DComponent>::GetID,
				"offset", sol::readonly(&BoxCollider2DComponent::m_Offset),
				"scale", sol::readonly(&BoxCollider2DComponent::m_Scale),
				"set_offset", sol::overload(
					[this](BoxCollider2DComponent& bc2d, float x, float y)
					{
						PhysicsSystem* physicsSys = PhysicsSystem::Instance();

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
						PhysicsSystem* physicsSys =PhysicsSystem::Instance();

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
						PhysicsSystem* physicsSys =PhysicsSystem::Instance();

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
						PhysicsSystem* physicsSys =PhysicsSystem::Instance();

						bc2d.m_Scale = newScale;

						if (bc2d.m_RuntimeFixture)
						{
							auto offset = physicsSys->PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
							auto scale = physicsSys->PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

							dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);
						}
					}
				)
			);*/
	}


	void BoxCollider2DSystem::CreateBoxCollider2DComponent(void* component, const nlohmann::json& json)
	{
		auto* comp = static_cast<BoxCollider2DComponent*>(component);
		JsonReader jsonWrapper(json);

		if (jsonWrapper.Empty() || !json.contains("colliders") || !json["colliders"].is_array())
			return;



		const auto& colliderArray = json["colliders"];

		comp->colliderCount = 0;

		for (const auto& entry : colliderArray)
		{
			if (comp->colliderCount >= MAX_COLLIDERS)
				break;

			BoxCollider2D box;
			JsonReader entryReader(entry);

			box.offset = entryReader.GetVec2("offset", { 0.0f, 0.0f });
			box.scale = entryReader.GetVec2("size", { 50.0f, 50.0f });
			box.density = entryReader.GetFloat("density", 1.0f);
			box.friction = entryReader.GetFloat("friction", 0.5f);
			box.restitution = entryReader.GetFloat("restitution", 0.0f);
			box.restitutionThreshold = entryReader.GetFloat("restitutionThreshold", 0.5f);

			const auto& filterJson = entry.contains("filter") ? entry["filter"] : nlohmann::json::object();
			box.filter.categoryBits = JsonReader(filterJson).GetUint16("categoryBits", 0x0001);
			box.filter.maskBits = JsonReader(filterJson).GetUint16("maskBits", 0xFFFF);
			box.filter.groupIndex = JsonReader(filterJson).GetInt16("groupIndex", 0);

			comp->colliders[comp->colliderCount++] = box;
		}



		ComponentFlagSet flags;
		flags.Set(ComponentFlags_Enabled);
		flags.Set(ComponentFlags_Serialized);

		comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

	}


	void BoxCollider2DSystem::AddBoxCollider2DComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<BoxCollider2DComponent*>(component);

		componentFactory->AddOrUpdate<BoxCollider2DComponent>(id, comp, registry, comp->colliders, comp->colliderCount, comp->runtimeBodyIndex, comp->componentFlags);
	}

	nlohmann::json BoxCollider2DSystem::WriteBoxCollider2DComponent(const void* component)
	{
		nlohmann::json json;
		auto& comp = *static_cast<const BoxCollider2DComponent*>(component);


		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["BoxCollider2D"]["colliders"] = nlohmann::json::array();

		for (uint8_t i = 0; i < comp.colliderCount; ++i)
		{
			const BoxCollider2D& box = comp.colliders[i];

			nlohmann::json colliderJson;
			colliderJson["offset"] = { box.offset.x, box.offset.y };
			colliderJson["size"] = { box.scale.x, box.scale.y };
			colliderJson["density"] = box.density;
			colliderJson["friction"] = box.friction;
			colliderJson["restitution"] = box.restitution;
			colliderJson["restitutionThreshold"] = box.restitutionThreshold;

			colliderJson["filter"] = {
				{ "categoryBits", box.filter.categoryBits },
				{ "maskBits", box.filter.maskBits },
				{ "groupIndex", box.filter.groupIndex }
			};

			json["BoxCollider2D"]["colliders"].push_back(colliderJson);
		}
		// Add component flags
		json["BoxCollider2D"]["ComponentFlags"] = comp.componentFlags.m_Flags;

		return json;
	}


	JSON BoxCollider2DSystem::DiffBoxCollider2DComponent(const void* base, const void* modified)
	{
		auto* b = static_cast<const BoxCollider2DComponent*>(modified);
		auto* a = static_cast<const BoxCollider2DComponent*>(base);

		JSON diff;
		//JSON colliderJson;

		//if (a->m_Offset != b->m_Offset)
		//	colliderJson["offset"] = { b->m_Offset.x, b->m_Offset.y };

		//if (a->m_Scale != b->m_Scale)
		//	colliderJson["scale"] = { b->m_Scale.x, b->m_Scale.y };

		//if (a->m_Density != b->m_Density)
		//	colliderJson["density"] = b->m_Density;

		//if (a->m_Friction != b->m_Friction)
		//	colliderJson["friction"] = b->m_Friction;

		//if (a->m_Restitution != b->m_Restitution)
		//	colliderJson["restitution"] = b->m_Restitution;

		//if (a->m_RestitutionThreshold != b->m_RestitutionThreshold)
		//	colliderJson["restitutionThreshold"] = b->m_RestitutionThreshold;

		//if (a->componentFlags.m_Flags != b->componentFlags.m_Flags)
		//	diff["ComponentFlags"] = b->componentFlags.m_Flags;

		//if (!colliderJson.empty())
		//	diff["BoxCollider2D"] = colliderJson;

		return diff;
	}


	void BoxCollider2DSystem::ViewBoxCollider2DComponent(Entity& entity)
	{
		BoxCollider2DComponent& bc2d = entity.Get<BoxCollider2DComponent>();
		PhysicsSystem* physicsSys = PhysicsSystem::Instance();


		uint8_t& flags = bc2d.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("BoxCollider2D", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<BoxCollider2DComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();


		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Body Index %d", bc2d.runtimeBodyIndex);


		for (uint8_t i = 0; i < bc2d.colliderCount; ++i)
		{
			BoxCollider2D& box = bc2d.colliders[i];

			if (!IsValidRuntimeIndex(box.runtimeFixtureIndex))
				continue;

			b2Fixture* fixture = physicsSys->GetActiveFixture(box.runtimeFixtureIndex);


			if (fixture)
			{

				uint8_t& cFlags = box.flags.m_Flags;

				auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Box Collider: " + std::to_string(i), cFlags, FixtureFlags_Enabled);

				if (remove)
				{
					if (fixture)
					{
						// Remove the fixture from the body
						b2Body* body = physicsSys->GetActiveBody(bc2d.runtimeBodyIndex);
						if (body)
						{
							body->DestroyFixture(fixture);
						}
					}
					// Remove the collider from the component
					bc2d.colliderCount--;
					for (uint8_t j = i; j < bc2d.colliderCount; ++j)
					{
						bc2d.colliders[j] = bc2d.colliders[j + 1];
					}
				}

				if (!open)
					continue;


				if (!enabled)
					ImGui::BeginDisabled();

				ImGui::PushID(i);
				//ImGui::Text("Collider %d", i + 1);




				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Fixture Index %d", box.runtimeFixtureIndex);



				bool isEnabled = box.flags.IsSet(FixtureFlags_Sensor);

				if (ImGui::Checkbox("Sensor", &isEnabled)) {
					if (isEnabled)
						box.flags.Set(FixtureFlags_Sensor);
					else
						box.flags.Clear(FixtureFlags_Sensor);
				}

				auto* shape = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
				if (shape)
				{
					bool changed = false;

					ImGui::PushID("OffsetEdit");
					changed |= ImGui::DragFloat2("Offset", glm::value_ptr(box.offset), 0.5f);
					bool finishedOffsetEdit = ImGui::IsItemDeactivatedAfterEdit();
					ImGui::PopID();

					ImGui::PushID("ScaleEdit");
					changed |= ImGui::DragFloat2("Scale", glm::value_ptr(box.scale), 0.5f);
					bool finishedScaleEdit = ImGui::IsItemDeactivatedAfterEdit();
					ImGui::PopID();

					// Only update when editing finishes
					if (finishedOffsetEdit || finishedScaleEdit)
					{
						// Recreate the fixture here
						b2Body* body = fixture->GetBody();

						body->DestroyFixture(fixture);
						fixture = nullptr;

						b2PolygonShape newShape;
						auto halfScale = PhysicsSystem::PixelsToMeters(box.scale.x / 2.0f, box.scale.y / 2.0f);
						auto offset = PhysicsSystem::PixelsToMeters(box.offset.x, box.offset.y);
						newShape.SetAsBox(halfScale.x, halfScale.y, b2Vec2(offset.x, offset.y), 0.0f);

						b2FixtureDef fixtureDef;
						fixtureDef.shape = &newShape;
						fixtureDef.density = 1.0f; // your value
						fixture = body->CreateFixture(&fixtureDef);

						physicsSys->UpdateActiveFixture(fixture, box.runtimeFixtureIndex);
					}
				}
				else
				{
					ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Warning: Collider is not a polygon shape.");
				}

				if (ImGui::DragFloat("Density", &box.density, 0.5f))
				{
					fixture->SetDensity(box.density);
					b2Body* body = fixture->GetBody();
					body->ResetMassData();
				}
				if (ImGui::DragFloat("Friction", &box.friction, 0.5f, 0, 1.0f))
					fixture->SetFriction(box.friction);
				if (ImGui::DragFloat("Restitution", &box.restitution, 0.5f))
					fixture->SetRestitution(box.restitution);
				if (ImGui::DragFloat("Restitution Threshold", &box.restitutionThreshold, 0.5f))
					fixture->SetRestitutionThreshold(box.restitutionThreshold);
				ImGui::PopID();

				if(!enabled)
					ImGui::EndDisabled();

				ImGui::TreePop();
			}




			//if (bc2d.m_RuntimeFixture)
			//{

			//	if (ImGui::DragFloat2("Offset", glm::value_ptr(bc2d.m_Offset), 0.5f))
			//	{
			//		auto scale = PhysicsSystem::PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
			//		auto offset = PhysicsSystem::PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);
			//		glm::vec3 childOffset(0.0f);

			//		CalculateOffset(childOffset, entity);

			//		auto childWorldPositionMeters = PhysicsSystem::PixelsToMeters(childOffset.x, childOffset.y);

			//		// Combine the child’s world position and the collider’s local offset
			//		b2Vec2 finalOffset(childWorldPositionMeters.x + offset.x,
			//			childWorldPositionMeters.y + offset.y);

			//		dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape())->SetAsBox(scale.x, scale.y, b2Vec2(finalOffset.x, finalOffset.y), 0.0f);

			//	}

			//	if (ImGui::DragFloat2("Scale", glm::value_ptr(bc2d.m_Scale), 0.5f))
			//	{
			//		auto scale = PhysicsSystem::PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);

			//		auto* shape = dynamic_cast<b2PolygonShape*>(bc2d.m_RuntimeFixture->GetShape());

			//		shape->SetAsBox(scale.x, scale.y, shape->m_centroid, 0.0f);
			//	}
			//	if (ImGui::DragFloat("Density", &bc2d.m_Density, 0.5f))
			//		bc2d.m_RuntimeFixture->SetDensity(bc2d.m_Density);

			//	if (ImGui::DragFloat("Friction", &bc2d.m_Friction, 0.5f, 0, 1.0f))
			//		bc2d.m_RuntimeFixture->SetFriction(bc2d.m_Friction);

			//	if (ImGui::DragFloat("Resitution", &bc2d.m_Restitution, 0.5f))
			//		bc2d.m_RuntimeFixture->SetRestitution(bc2d.m_Restitution);

			//	if (ImGui::DragFloat("Resitution Threshold", &bc2d.m_RestitutionThreshold, 0.5f))
			//		bc2d.m_RuntimeFixture->SetRestitutionThreshold(bc2d.m_RestitutionThreshold);

			//}
			//else
			//{
			//	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires Rigidbody2D component to enable physics");
			//}
		}

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 buttonSize = ImVec2(150, 25);
		ImVec2 buttonPos = ImVec2(cursorPos.x + (windowSize.x - buttonSize.x) * 0.5f, cursorPos.y);
		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Add Component", buttonSize))
		{
			if (bc2d.colliderCount < MAX_COLLIDERS)
			{
				AddCollider(bc2d, entity);
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Maximum number of colliders reached");
			}
		}

		if (!enabled)
			ImGui::EndDisabled();


		ImGui::TreePop();
	}

	void BoxCollider2DSystem::CalculateOffset(glm::vec3& offset, Entity& entity)
	{
		auto* sceneManager = NSceneManager::Instance();

		if (entity.Has<ParentComponent>())
		{
			auto& pComp = entity.Get<ParentComponent>();

			Entity& parent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

			if (parent.Has<Rigidbody2DComponent>())
			{
				auto& parentTransform = parent.Get<TransformComponent>();
				auto& transform = entity.Get<TransformComponent>();

				auto rotation = transform.rotation;
				auto translation = transform.translation;

				PhysicsSystem::LocalToWorldPos(transform, translation, rotation);

				offset = translation - parentTransform.translation;
			}
			else
			{
				CalculateOffset_rec(offset, entity, parent, sceneManager);
			}
		}
	}


	void BoxCollider2DSystem::AddCollider(BoxCollider2DComponent& collider, Entity& ent)
	{
		TransformComponent& transform = ent.Get<TransformComponent>();
		BoxCollider2D box;
		auto* body = PhysicsSystem::Instance()->GetActiveBody(collider.runtimeBodyIndex);
		auto* PhysicsSystem = PhysicsSystem::Instance();
		if (!body)
			return;

		b2PolygonShape boxShape;

		auto scale = PhysicsSystem::PixelsToMeters(box.scale.x / 2, box.scale.y / 2);
		auto offset = PhysicsSystem::PixelsToMeters(box.offset.x, box.offset.y);


		glm::vec3 childOffset(0.0f);
		CalculateOffset(childOffset, ent);
		auto childWorldPositionMeters = PhysicsSystem::PixelsToMeters(childOffset.x, childOffset.y);
		// Combine the child’s world position and the collider’s local offset
		b2Vec2 finalOffset(childWorldPositionMeters.x + offset.x,
			childWorldPositionMeters.y + offset.y);

		boxShape.SetAsBox(scale.x, scale.y, b2Vec2(finalOffset.x, finalOffset.y), 0.0f);

		b2FixtureDef fixDef;

		fixDef.shape = &boxShape;

		fixDef.shape = &boxShape;
		fixDef.density = box.density;
		fixDef.friction = box.friction;
		fixDef.restitution = box.restitution;
		fixDef.restitutionThreshold = box.restitutionThreshold;
		fixDef.isSensor = box.flags.IsSet(FixtureFlags_Sensor);

		box.runtimeFixtureIndex = PhysicsSystem->AddActiveFixture(PhysicsSystem->CreateFixture(body, fixDef));

		collider.colliders[collider.colliderCount++] = box;
	}


	bool BoxCollider2DSystem::InitializeCollider(EntityID entityID, NRegistry* registry)
	{
		if (!registry->HasComponent<BoxCollider2DComponent>(entityID))
			return true;

		auto* PhysicsSystem = PhysicsSystem::Instance();

		BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(entityID);
		TransformComponent& transform = registry->GetComponent<TransformComponent>(entityID);

		if (!IsValidRuntimeIndex(bc2d.runtimeBodyIndex))
		{
			if (registry->HasComponent<Rigidbody2DComponent>(entityID))
				bc2d.runtimeBodyIndex = registry->GetComponent<Rigidbody2DComponent>(entityID).runtimeBodyIndex;
			else if (registry->HasComponent<CircleCollider2DComponent>(entityID))
				bc2d.runtimeBodyIndex = registry->GetComponent<CircleCollider2DComponent>(entityID).runtimeBodyIndex;

			// If the body index is still invalid, we have to create a body
			if (!IsValidRuntimeIndex(bc2d.runtimeBodyIndex))
			{
				b2BodyDef bodyDef;
				auto pos = PhysicsSystem::PixelsToMeters(transform.translation.x, transform.translation.y);
				bodyDef.position.Set(pos.x, pos.y);

				bc2d.runtimeBodyIndex = PhysicsSystem->AddActiveBody(PhysicsSystem->CreateBody(bodyDef));
			}
		}

		// If the body index is still invalid, we have to create a body
		{

			auto* body = PhysicsSystem::Instance()->GetActiveBody(bc2d.runtimeBodyIndex);


			NLE_CORE_ASSERT(body != nullptr, "BoxCollider2DSystem::InitializeCollider: Body is null, cannot initialize collider");




			for (uint8_t i = 0; i < bc2d.colliderCount; ++i)
			{
				BoxCollider2D& box = bc2d.colliders[i];

				if(IsValidRuntimeIndex(box.runtimeFixtureIndex))
					return true; // Already initialized

	

				b2PolygonShape boxShape;

				auto offset = PhysicsSystem::PixelsToMeters(box.offset.x, box.offset.y);
				auto scale = PhysicsSystem::PixelsToMeters(box.scale.x / 2, box.scale.y / 2);


				boxShape.SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);


				b2FixtureDef fixDef;

				fixDef.shape = &boxShape;
				fixDef.density = box.density;
				fixDef.friction = box.friction;
				fixDef.restitution = box.restitution;
				fixDef.restitutionThreshold = box.restitutionThreshold;
				fixDef.isSensor = box.flags.IsSet(FixtureFlags_Sensor);

				box.runtimeFixtureIndex = PhysicsSystem->AddActiveFixture(PhysicsSystem->CreateFixture(body, fixDef));

			}

			//body->ResetMassData(); // Reset mass data to ensure correct physics calculations

		}


		return true;
	}


	bool BoxCollider2DSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NEventManager* eventManager = NEventManager::Instance();
		NRegistry* registry = NRegistry::Instance();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			eventManager->QueueAsync(std::make_unique<InitializeBox2DColliderEvent>(e.GetID()));


		return true;
	}


	bool BoxCollider2DSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();


		// IF WE ARE REMOVING A CIRCLE COLLIDER 2D COMPONENT
		if (e.GetComponentID() == Component<BoxCollider2DComponent>::GetID())
		{
			/*
					return true;*/

			BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(e.GetID());


			// remove all colliders
			for (uint8_t i = 0; i < bc2d.colliderCount; ++i)
			{
				if (IsValidRuntimeIndex(bc2d.colliders[i].runtimeFixtureIndex))
				{
					b2Body* body = PhysicsSystem::Instance()->GetActiveBody(bc2d.runtimeBodyIndex);
					if (body)
					{
						PhysicsSystem::Instance()->RemoveActiveFixture(bc2d.colliders[i].runtimeFixtureIndex, body);
					}
				}
			}

			// remove body if no collider or rigidbody left
			if (!registry->HasComponent<Rigidbody2DComponent>(e.GetID()) && !registry->HasComponent<BoxCollider2DComponent>(e.GetID()))
				PhysicsSystem::Instance()->RemoveActiveBody(bc2d.runtimeBodyIndex);


		}

		return true;
	}



	bool BoxCollider2DSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		const auto& entityList = GetSystemEntities();
		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
		{
			// if the entity is not in the system, we need to add it
			if (e.GetComponentID() == Component<BoxCollider2DComponent>::GetID())
			{
				eventManager->QueueAsync(std::make_unique<InitializeBox2DColliderEvent>(e.GetID()));
			}
		}

		return true;
	}

	bool BoxCollider2DSystem::OnInitializeBox2DStart(const InitializeBox2DColliderEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		return InitializeCollider(e.GetEntityID(), registry);
	}

	void BoxCollider2DSystem::CalculateOffset_rec(glm::vec3& offset, Entity& entity, Entity& parent, NSceneManager* sceneManager)
	{
		if (parent.Has<ParentComponent>())
		{
			auto& pComp = parent.Get<ParentComponent>();

			Entity& grandParent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

			if (grandParent.Has<Rigidbody2DComponent>())
			{
				auto& parentTransform = grandParent.Get<TransformComponent>();
				auto& transform = entity.Get<TransformComponent>();

				auto rotation = transform.rotation;
				auto translation = transform.translation;

				PhysicsSystem::LocalToWorldPos(transform, translation, rotation);

				offset = translation - parentTransform.translation;
			}
			else
			{
				CalculateOffset_rec(offset, entity, grandParent, sceneManager);
			}
		}
	}


}