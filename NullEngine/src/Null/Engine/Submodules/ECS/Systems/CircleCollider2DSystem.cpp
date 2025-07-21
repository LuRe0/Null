
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
#include "../Entities/Entity.h"
#include "../../../Modules/NSceneManager.h"
#include "../../Scene.h"
#include "../../../../Tools/ImGuiH.h"

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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<CircleCollider2DComponent>(CreateCircleCollider2DComponent,
			[this](Entity& id) { this->ViewCircleCollider2DComponent(id); },
			WriteCircleCollider2DComponent,
			AddCircleCollider2DComponent, DiffCircleCollider2DComponent);
	}

	void CircleCollider2DSystem::Load()
	{
	}

	void CircleCollider2DSystem::Init()
	{
		ISystem::Init();


		NEventManager* eventManager = NEventManager::Instance();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &CircleCollider2DSystem::OnEntityCreated, eventManager, EventPriority::Low);
		//SUBSCRIBE_EVENT(EntityDestroyedEvent, &CircleCollider2DSystem::OnEntityDestroyed, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &CircleCollider2DSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &CircleCollider2DSystem::OnEntityComponentAdded, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(SceneSwitchEvent, &CircleCollider2DSystem::OnSceneSwitched, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(InitializeBox2DColliderEvent, &CircleCollider2DSystem::OnInitializeBox2DStart, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntityParentedEvent, &PhysicsSystem::OnEntityParented, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntitySeparatedEvent, &PhysicsSystem::OnEntitySeparated, eventManager, EventPriority::High);

		NRegistry* registry = NRegistry::Instance();


		for (const auto entityId : GetSystemEntities())
		{
			eventManager->QueueEvent(std::make_unique<InitializeBox2DColliderEvent>(entityId));
		}
	}

	void CircleCollider2DSystem::Update(float dt)
	{

	}

	void CircleCollider2DSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::ColorEdit4("Collider Color", glm::value_ptr(m_Color));
		ImGui::DragFloat("Collider Thickness", &m_Thickness, 0.5f);
	}

	void CircleCollider2DSystem::Render()
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
			CircleCollider2DComponent& cc2D = m_Parent->GetComponent<CircleCollider2DComponent>(entityId);

			if (!cc2D.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			b2Body* body = PhysicsSystem::Instance()->GetActiveBody(cc2D.runtimeBodyIndex);

			if (!body)
				continue;

			for (size_t i = 0; i < cc2D.colliderCount; i++)
			{
				const CircleCollider2D& collider = cc2D.colliders[i];

				auto translation = camManager->GetCurrentCamera()->GetCameraType() == Camera::PERSPECTIVE ?
					(transform.translation + glm::vec3(collider.offset, transform.translation.z + transform.scale.z + 0.50f)) :
					(transform.translation + glm::vec3(collider.offset, transform.translation.z + 0.50f));

				auto rot = body->GetAngle();


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

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(collider.radius, collider.radius, 1.0f));

				glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;


				/*		matrix, meshManager->Get("Quad"), "", glm::vec4(0, 1, 0, 1), "",
							0, entityId, 0.05f, 0.005f, RenderData::INSTANCED)*/
							//model, mesh, spritesrc, tint, shadername, frameindex, entity
				renderer->AddRenderCall(RenderCommandTypes::Debug, std::make_unique<ElementData>(matrix, meshManager->Get("Circle"), nullptr, m_Color, "", 0,
					entityId, m_Thickness, 0.005f, RenderData::INSTANCED, -depth));

			}
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
		//lua.new_usertype<CircleCollider2DComponent>
		//	(
		//		"BoxCollider2D",
		//		sol::no_constructor,
		//		"type_id", &Component<CircleCollider2DComponent>::GetID,
		//		"offset", sol::readonly(&CircleCollider2DComponent::m_Offset),
		//		"radius", sol::readonly(&CircleCollider2DComponent::m_Radius),
		//		"set_offset", sol::overload(
		//			[this](CircleCollider2DComponent& cc2d, float x, float y)
		//			{
		//				PhysicsSystem* physicsSys =PhysicsSystem::Instance();

		//				cc2d.m_Offset = glm::vec2(x, y);

		//				if (cc2d.m_RuntimeFixture)
		//				{
		//					auto offset = physicsSys->PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
		//					dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_p.Set(offset.x, offset.y);
		//				}
		//			},
		//			[this](CircleCollider2DComponent& cc2d, glm::vec2 newOffset)
		//			{
		//				PhysicsSystem* physicsSys =PhysicsSystem::Instance();

		//				cc2d.m_Offset = newOffset;

		//				if (cc2d.m_RuntimeFixture)
		//				{
		//					auto offset = physicsSys->PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
		//					dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_p.Set(offset.x, offset.y);
		//				}
		//			}
		//		),
		//		"set_radius", [this](CircleCollider2DComponent& cc2d, float r)
		//		{
		//			PhysicsSystem* physicsSys =PhysicsSystem::Instance();

		//			cc2d.m_Radius = r;

		//			if (cc2d.m_RuntimeFixture)
		//			{
		//				auto radius = physicsSys->PixelsToMeters(cc2d.m_Radius);

		//				dynamic_cast<b2CircleShape*>(cc2d.m_RuntimeFixture->GetShape())->m_radius = radius;
		//			}
		//		}
		//);
	}


	void CircleCollider2DSystem::AddCollider(CircleCollider2DComponent& collider)
	{
		CircleCollider2D circle;
		auto* body = PhysicsSystem::Instance()->GetActiveBody(collider.runtimeBodyIndex);
		auto* PhysicsSystem = PhysicsSystem::Instance();
		if (!body)
			return;

		b2CircleShape circleShape;

		auto offset = PhysicsSystem::PixelsToMeters(circle.offset.x, circle.offset.y);
		auto radius = PhysicsSystem::PixelsToMeters(circle.radius);


		circleShape.m_p.Set(offset.x, offset.y);
		circleShape.m_radius = radius;

		b2FixtureDef fixDef;

		fixDef.shape = &circleShape;
		fixDef.density = circle.density;
		fixDef.friction = circle.friction;
		fixDef.restitution = circle.restitution;
		fixDef.restitutionThreshold = circle.restitutionThreshold;

		circle.runtimeFixtureIndex = PhysicsSystem->AddActiveFixture(PhysicsSystem->CreateFixture(body, fixDef));

		collider.colliders[collider.colliderCount++] = circle;
	}

	void CircleCollider2DSystem::CreateCircleCollider2DComponent(void* component, const nlohmann::json& json)
	{
		auto* comp = static_cast<CircleCollider2DComponent*>(component);
		JsonReader jsonWrapper(json);

		if (jsonWrapper.Empty() || !json.contains("colliders") || !json["colliders"].is_array())
			return;

		const auto& colliderArray = json["colliders"];
		comp->colliderCount = 0;

		for (const auto& entry : colliderArray)
		{
			if (comp->colliderCount >= MAX_COLLIDERS)
				break;

			CircleCollider2D circle;
			JsonReader entryReader(entry);

			circle.offset = entryReader.GetVec2("offset", { 0.0f, 0.0f });
			circle.radius = entryReader.GetFloat("radius", 50.0f);
			circle.density = entryReader.GetFloat("density", 1.0f);
			circle.friction = entryReader.GetFloat("friction", 0.5f);
			circle.restitution = entryReader.GetFloat("restitution", 0.0f);
			circle.restitutionThreshold = entryReader.GetFloat("restitutionThreshold", 0.5f);

			const auto& filterJson = entry.contains("filter") ? entry["filter"] : nlohmann::json::object();
			circle.filter.categoryBits = JsonReader(filterJson).GetUint16("categoryBits", 0x0001);
			circle.filter.maskBits = JsonReader(filterJson).GetUint16("maskBits", 0xFFFF);
			circle.filter.groupIndex = JsonReader(filterJson).GetInt16("groupIndex", 0);

			comp->colliders[comp->colliderCount++] = circle;
		}


		ComponentFlagSet flags;
		flags.Set(ComponentFlags_Enabled);
		flags.Set(ComponentFlags_Serialized);

		comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
	}


	void CircleCollider2DSystem::AddCircleCollider2DComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<CircleCollider2DComponent*>(component);
		componentFactory->AddOrUpdate<CircleCollider2DComponent>(id, comp, registry, comp->colliders, comp->colliderCount, comp->runtimeBodyIndex,
			comp->componentFlags);
	}

	nlohmann::json CircleCollider2DSystem::WriteCircleCollider2DComponent(const void* component)
	{
		nlohmann::json json;
		auto& comp = *static_cast<const CircleCollider2DComponent*>(component);

		json["CircleCollider2D"]["colliders"] = nlohmann::json::array();

		for (uint8_t i = 0; i < comp.colliderCount; ++i)
		{
			const CircleCollider2D& circle = comp.colliders[i];

			nlohmann::json colliderJson;
			colliderJson["offset"] = { circle.offset.x, circle.offset.y };
			colliderJson["radius"] = circle.radius;
			colliderJson["density"] = circle.density;
			colliderJson["friction"] = circle.friction;
			colliderJson["restitution"] = circle.restitution;
			colliderJson["restitutionThreshold"] = circle.restitutionThreshold;

			colliderJson["filter"] = {
				{ "categoryBits", circle.filter.categoryBits },
				{ "maskBits", circle.filter.maskBits },
				{ "groupIndex", circle.filter.groupIndex }
			};

			json["CircleCollider2D"]["colliders"].push_back(colliderJson);
		}

		json["CircleCollider2D"]["ComponentFlags"] = comp.componentFlags.m_Flags;


		return json;
	}


	JSON CircleCollider2DSystem::DiffCircleCollider2DComponent(const void* base, const void* modified)
	{
		auto* a = static_cast<const CircleCollider2DComponent*>(base);
		auto* b = static_cast<const CircleCollider2DComponent*>(modified);

		JSON diff;
		JSON circleJson;

		//if (a->m_Offset != b->m_Offset)
		//	circleJson["offset"] = { b->m_Offset.x, b->m_Offset.y };

		//if (a->m_Radius != b->m_Radius)
		//	circleJson["radius"] = b->m_Radius;

		//if (a->m_Density != b->m_Density)
		//	circleJson["density"] = b->m_Density;

		//if (a->m_Friction != b->m_Friction)
		//	circleJson["friction"] = b->m_Friction;

		//if (a->m_Restitution != b->m_Restitution)
		//	circleJson["restitution"] = b->m_Restitution;

		//if (a->m_RestitutionThreshold != b->m_RestitutionThreshold)
		//	circleJson["restitutionThreshold"] = b->m_RestitutionThreshold;

		//if (!circleJson.empty())
		//	diff["CircleCollider2D"] = circleJson;

		return diff;
	}


	void CircleCollider2DSystem::ViewCircleCollider2DComponent(Entity& entity)
	{
		CircleCollider2DComponent& cc2d = entity.Get<CircleCollider2DComponent>();
		PhysicsSystem* physicsSys = PhysicsSystem::Instance();


		uint8_t& flags = cc2d.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("CircleCollider2D", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<CircleCollider2DComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		for (uint8_t i = 0; i < cc2d.colliderCount; ++i)
		{
			CircleCollider2D& circle = cc2d.colliders[i];

			if (!IsValidRuntimeIndex(circle.runtimeFixtureIndex))
				continue;

			b2Fixture* fixture = physicsSys->GetActiveFixture(circle.runtimeFixtureIndex);


			if (fixture)
			{
			
				auto [open, remove] = ImGuiH::CollapsingHeaderWithRemove("Circle Collider" + std::to_string(i));

				if (remove)
				{
					auto* fixture = physicsSys->GetActiveFixture(circle.runtimeFixtureIndex);
					if (fixture)
					{
						// Remove the fixture from the body
						b2Body* body = physicsSys->GetActiveBody(cc2d.runtimeBodyIndex);
						if (body)
						{
							body->DestroyFixture(fixture);
						}
					}
					// Remove the collider from the component
					cc2d.colliderCount--;
					for (uint8_t j = i; j < cc2d.colliderCount; ++j)
					{
						cc2d.colliders[j] = cc2d.colliders[j + 1];
					}
				}

				if (!open)
					continue;

				ImGui::PushID(i);
				ImGui::Text("Collider %d", i + 1);
				if (ImGui::DragFloat2("Offset", glm::value_ptr(circle.offset), 0.5f))
				{
					auto offset = physicsSys->PixelsToMeters(circle.offset.x, circle.offset.y);
					glm::vec3 childOffset(0.0f);
					CalculateOffset(childOffset, entity);
					auto childWorldPositionMeters = PhysicsSystem::PixelsToMeters(childOffset.x, childOffset.y);
					// Combine the child’s world position and the collider’s local offset
					b2Vec2 finalOffset(childWorldPositionMeters.x + offset.x,
						childWorldPositionMeters.y + offset.y);
					dynamic_cast<b2CircleShape*>(fixture->GetShape())->m_p.Set(finalOffset.x, finalOffset.y);
				}
				if (ImGui::DragFloat("Radius", &circle.radius, 0.5f))
				{
					auto radius = PhysicsSystem::PixelsToMeters(circle.radius);
					dynamic_cast<b2CircleShape*>(fixture->GetShape())->m_radius = radius;
				}
				if (ImGui::DragFloat("Density", &circle.density, 0.5f))
					fixture->SetDensity(circle.density);
				if (ImGui::DragFloat("Friction", &circle.friction, 0.5f, 0, 1.0f))
					fixture->SetFriction(circle.friction);
				if (ImGui::DragFloat("Restitution", &circle.restitution, 0.5f))
					fixture->SetRestitution(circle.restitution);
				if (ImGui::DragFloat("Restitution Threshold", &circle.restitutionThreshold, 0.5f))
					fixture->SetRestitutionThreshold(circle.restitutionThreshold);
				ImGui::PopID();


				ImGui::TreePop();
			}
		}

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 buttonSize = ImVec2(150, 25);
		ImVec2 buttonPos = ImVec2(cursorPos.x + (windowSize.x - buttonSize.x) * 0.5f, cursorPos.y);
		ImGui::SetCursorPos(buttonPos);

		if (ImGui::Button("Add Component", buttonSize))
		{
			if (cc2d.colliderCount < MAX_COLLIDERS)
			{
				AddCollider(cc2d);
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

	bool CircleCollider2DSystem::InitializeCollider(EntityID entityID, NRegistry* registry)
	{
		if (!registry->HasComponent<CircleCollider2DComponent>(entityID))
			return true;

		auto* PhysicsSystem = PhysicsSystem::Instance();

		CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(entityID);

		if (!IsValidRuntimeIndex(cc2d.runtimeBodyIndex))
		{
			if (registry->HasComponent<Rigidbody2DComponent>(entityID))
				cc2d.runtimeBodyIndex = registry->GetComponent<Rigidbody2DComponent>(entityID).runtimeBodyIndex;
			else if (registry->HasComponent<BoxCollider2DComponent>(entityID))
				cc2d.runtimeBodyIndex = registry->GetComponent<BoxCollider2DComponent>(entityID).runtimeBodyIndex;
			// If the body index is still invalid, we have to create a body
			if (!IsValidRuntimeIndex(cc2d.runtimeBodyIndex))
			{
				b2BodyDef bodyDef;
				cc2d.runtimeBodyIndex = PhysicsSystem->AddActiveBody(PhysicsSystem->CreateBody(bodyDef));
			}
		}

		// If the body is not valid, we need to create it

		{
			auto* body = PhysicsSystem::Instance()->GetActiveBody(cc2d.runtimeBodyIndex);

			NLE_CORE_ASSERT(body, "CircleCollider2DSystem::InitializeCollider: Body is null for entity ID: {0}", entityID);

			body->ResetMassData(); // Reset mass data to ensure correct physics calculations


			for (uint8_t i = 0; i < cc2d.colliderCount; ++i)
			{
				CircleCollider2D& circle = cc2d.colliders[i];

				if (IsValidRuntimeIndex(circle.runtimeFixtureIndex))
					return true; // Already initialized


				b2CircleShape circleShape;

				auto offset = PhysicsSystem::PixelsToMeters(circle.offset.x, circle.offset.y);
				auto radius = PhysicsSystem::PixelsToMeters(circle.radius);


				circleShape.m_p.Set(offset.x, offset.y);
				circleShape.m_radius = radius;

				b2FixtureDef fixDef;

				fixDef.shape = &circleShape;
				fixDef.density = circle.density;
				fixDef.friction = circle.friction;
				fixDef.restitution = circle.restitution;
				fixDef.restitutionThreshold = circle.restitutionThreshold;

				circle.runtimeFixtureIndex = PhysicsSystem->AddActiveFixture(PhysicsSystem->CreateFixture(body, fixDef));
			}
		}


		return true;
	}



	void CircleCollider2DSystem::CalculateOffset(glm::vec3& offset, Entity& entity)
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

	void CircleCollider2DSystem::CalculateOffset_rec(glm::vec3& offset, Entity& entity, Entity& parent, NSceneManager* sceneManager)
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


	bool CircleCollider2DSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NEventManager* eventManager = NEventManager::Instance();
		NRegistry* registry = NRegistry::Instance();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			eventManager->QueueAsync(std::make_unique<InitializeBox2DColliderEvent>(e.GetID()));


		return true;
	}



	bool CircleCollider2DSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();


		// IF WE ARE REMOVING A CIRCLE COLLIDER 2D COMPONENT
		if (e.GetComponentID() == Component<CircleCollider2DSystem>::GetID())
		{
			/*
					return true;*/

			CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(e.GetID());


			// remove all colliders
			for (uint8_t i = 0; i < cc2d.colliderCount; ++i)
			{
				if (IsValidRuntimeIndex(cc2d.colliders[i].runtimeFixtureIndex))
				{
					b2Body* body = PhysicsSystem::Instance()->GetActiveBody(cc2d.runtimeBodyIndex);
					if (body)
					{
						PhysicsSystem::Instance()->RemoveActiveFixture(cc2d.colliders[i].runtimeFixtureIndex, body);
					}
				}
			}

			// remove body if no collider or rigidbody left
			if (!registry->HasComponent<Rigidbody2DComponent>(e.GetID()) && !registry->HasComponent<BoxCollider2DComponent>(e.GetID()))
				PhysicsSystem::Instance()->RemoveActiveBody(cc2d.runtimeBodyIndex);


		}

		return true;
	}



	bool CircleCollider2DSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		const auto& entityList = GetSystemEntities();
		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
		{
			// if the entity is not in the system, we need to add it
			if (e.GetComponentID() == Component<CircleCollider2DComponent>::GetID())
			{
				eventManager->QueueAsync(std::make_unique<InitializeBox2DColliderEvent>(e.GetID()));
			}
		}

		return true;
	}

	bool CircleCollider2DSystem::OnInitializeBox2DStart(const InitializeBox2DColliderEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		return InitializeCollider(e.GetEntityID(), registry);
	}

}