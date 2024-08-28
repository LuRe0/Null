
//------------------------------------------------------------------------------
//
// File Name:	PhysicsSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "PhysicsSystem.h"
#include "imgui.h"
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <magic_enum/magic_enum.hpp>
#include "Null/Engine/Submodules/Events/IEvents.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	float PhysicsSystem::m_Pixels_Per_Meter = 64.0f;

	PhysicsSystem::PhysicsSystem() : m_PhysicsWorld(nullptr)
	{
		Require<TransformComponent>();
		Require<Rigidbody2DComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<Rigidbody2DComponent>(CreateRigidbody2DComponent,
			[this](Entity& id) { this->ViewRigidbody2DComponent(id); }, WriteRigidbody2DComponent);
	}
	void PhysicsSystem::Load()
	{
		auto gravity = PixelsToMeters(GRAVITY.x, GRAVITY.y);
		m_PhysicsWorld = new b2World({ gravity.x, gravity.y });
	}

	void PhysicsSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &PhysicsSystem::OnEntityCreated, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &PhysicsSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &PhysicsSystem::OnEntityComponentAdded, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &PhysicsSystem::OnSceneSwitched, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(InitializeBox2DEvent, &PhysicsSystem::OnSceneStart, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityParentedEvent, &PhysicsSystem::OnEntityParented, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntitySeparatedEvent, &PhysicsSystem::OnEntitySeparated, eventManager, EventPriority::High);

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();


		for (const auto entityId : GetSystemEntities())
		{
			eventManager->QueueEvent(std::make_unique<InitializeBox2DEvent>(entityId));
		}

	}

	void PhysicsSystem::Update(float dt)
	{
		if (m_Simulate)
		{
			RuntimeUpdate(dt);
		}
	}

	void PhysicsSystem::RuntimeUpdate(float dt)
	{
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);


			if (!transform.m_DirectManipulation)
				continue;

			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);
			b2Body* body = rb2d.m_RuntimeBody;

			if (body)
			{

				auto rotation = transform.m_Rotation;
				auto translation = transform.m_Translation;

				if (m_Parent->HasComponent<ParentComponent>(entityId))
				{
					//auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

					//TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);

					LocalToWorldPos(transform, translation, rotation);
				}


				auto pos = PixelsToMeters(translation.x, translation.y);
				body->SetTransform({ pos.x, pos.y }, rotation.z);

				transform.m_DirectManipulation = false;
			}

		}


		m_PhysicsWorld->Step(dt, velocityIterations, positionIterations);

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			b2Body* body = rb2d.m_RuntimeBody;


			if (body)
			{

				body->SetAwake(rb2d.m_Enabled);

				if (!rb2d.m_Enabled)
					continue;

				const auto& position = body->GetPosition();

				auto newPos = MetersToPixels(position.x, position.y);

				auto linearVel = body->GetLinearVelocity();

				rb2d.m_LinearVelocity = MetersToPixels(linearVel.x, linearVel.y);

				transform.m_Translation.x = newPos.x;
				transform.m_Translation.y = newPos.y;

				transform.m_Rotation.z = body->GetAngle();

				if (m_Parent->HasComponent<ParentComponent>(entityId))
				{
					auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

					TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);
					WorldToLocalPos(transform, parentTransform);
				}


				transform.m_Dirty = true;
			}
		}
	}

	void PhysicsSystem::Render()
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
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			if (!rb2d.m_RuntimeBody)
				continue;

			// Define a maximum length for the line
			float maxLength = 250.0f; // Example max length; adjust as needed

			// Calculate the current length of the velocity vector
			float velocityLength = glm::length(rb2d.m_LinearVelocity);

			// Define a reference speed for scaling
			float referenceSpeed = 1000.0f; // Example reference speed; adjust as needed

			// Calculate the scaling factor based on the ratio of velocity to the reference speed
			float scaleFactor = glm::min(velocityLength / referenceSpeed, 1.0f);

			// Determine the actual length of the line based on the scale factor and maximum length
			float lineLength = maxLength * scaleFactor;

			// Normalize the velocity vector and calculate the offset
			auto offset = glm::normalize(rb2d.m_LinearVelocity) * (lineLength / 2.0f);

			// Calculate the translation with the correct depth
			//auto translation = glm::vec3(transform.m_Translation.x, transform.m_Translation.y, (transform.m_Translation.z + transform.m_Scale.z * 0.5f + 0.50f));
			auto translation = (transform.m_Translation + glm::vec3(offset, transform.m_Translation.z + transform.m_Scale.z + 0.50f));

			// Calculate the angle of rotation based on the linear velocity vector
			float rot = glm::atan(rb2d.m_LinearVelocity.y, rb2d.m_LinearVelocity.x);

			glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * glm::vec4(translation, 1.0f);

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;

			// Create the translation matrix
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

			// Create the rotation matrix, rotating around the Z-axis based on the velocity direction
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 0, 1));

			// Create the scale matrix based on the length of the velocity vector
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(lineLength, m_Thickness, 1.0f));

			// Combine the matrices to form the final transformation matrix
			glm::mat4 matrix = translationMatrix * rotationMatrix * scaleMatrix;

			// Add the render call for the line
			renderer->AddDebugRenderCall(std::make_unique<ElementData>(matrix, meshManager->Get("Line"), nullptr, m_Color, "", 0,
				entityId, m_Thickness, 0.005f, RenderData::INSTANCED, depth));
		}
	}

	void PhysicsSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::Text("Pixels Per Meter:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f); // Set the width of the DragFloat
		ImGui::DragFloat("##Pixels Per Meter", &m_Pixels_Per_Meter, 0.5f, 4, 128);

		ImGui::ColorEdit4("Vector Color", glm::value_ptr(m_Color), 0.5f);
	}

	void PhysicsSystem::Unload()
	{
		delete m_PhysicsWorld;
	}

	void PhysicsSystem::Shutdown()
	{
	}

	void PhysicsSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<Rigidbody2DComponent>
			(
				"Rigidbody2D",
				sol::no_constructor,
				"type_id", &Component<Rigidbody2DComponent>::GetID,
				"linear_velocity", sol::readonly(&Rigidbody2DComponent::m_LinearVelocity),
				"angular_velocity", sol::readonly(&Rigidbody2DComponent::m_AngularVelocity),
				"angular_damping", sol::readonly(&Rigidbody2DComponent::m_AngularDamping),
				"linear_damping", sol::readonly(&Rigidbody2DComponent::m_LinearDamping),
				"gravity_scale", sol::readonly(&Rigidbody2DComponent::m_GravityScale),
				"set_linear_velocity", sol::overload(
					[this](Rigidbody2DComponent& rb2d, float x, float y)
					{
						rb2d.m_LinearVelocity = glm::vec2(x, y);

						auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

						if (rb2d.m_RuntimeBody)
							rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });
					},
					[this](Rigidbody2DComponent& rb2d, glm::vec2 newVel)
					{
						rb2d.m_LinearVelocity = newVel;

						auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

						if (rb2d.m_RuntimeBody)
							rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });
					}
				),
				"set_angular_velocity", [](Rigidbody2DComponent& rb2d, float v)
				{
					rb2d.m_AngularVelocity = v;
					if (rb2d.m_RuntimeBody)
						rb2d.m_RuntimeBody->SetAngularVelocity(rb2d.m_AngularVelocity);
				},
				"set_gravity_scale", [](Rigidbody2DComponent& rb2d, float g)
				{
					rb2d.m_GravityScale = g;
					if (rb2d.m_RuntimeBody)
						rb2d.m_RuntimeBody->SetGravityScale(rb2d.m_GravityScale);
				}
		);
	}


	// Converts pixel values to meter values
	const glm::vec2 PhysicsSystem::PixelsToMeters(float xPixels, float yPixels)
	{
		float xMeters = xPixels / m_Pixels_Per_Meter; // Convert pixels to meters
		float yMeters = yPixels / m_Pixels_Per_Meter; // Convert pixels to meters

		return glm::vec2(xMeters, yMeters);
	}

	const float PhysicsSystem::MetersToPixels(float meters)
	{
		return meters * m_Pixels_Per_Meter;
	}

	const float PhysicsSystem::PixelsToMeters(float pixels)
	{
		return pixels / m_Pixels_Per_Meter;
	}

	// Converts meter values to pixel values
	const glm::vec2 PhysicsSystem::MetersToPixels(float xMeters, float yMeters)
	{
		float xPixels = xMeters * m_Pixels_Per_Meter; // Convert meters to pixels
		float yPixels = yMeters * m_Pixels_Per_Meter; // Convert meters to pixels

		return glm::vec2(xPixels, yPixels);
	}




	void PhysicsSystem::CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<Rigidbody2DComponent*>(component);
		JsonReader jsonWrapper(json);


		if (!jsonWrapper.Empty())
		{
			comp->m_Type = static_cast<Rigidbody2DComponent::BodyType>(jsonWrapper.GetInt("type", 0));
			comp->m_FixedRotation = jsonWrapper.GetBool("fixedRotation", true);
			comp->m_LinearVelocity = jsonWrapper.GetVec2("linearVelocity", glm::vec2(0, 0));
			comp->m_AngularVelocity = jsonWrapper.GetFloat("angularVelocity", 0.0f);
			comp->m_LinearDamping = jsonWrapper.GetFloat("linearDamping", 0.0f);
			comp->m_AngularDamping = jsonWrapper.GetFloat("angularDamping", 0.0f);
			comp->m_GravityScale = jsonWrapper.GetFloat("gravityScale", 1.0f);
		}

		componentFactory->AddOrUpdate<Rigidbody2DComponent>(id, comp, registry, static_cast<Rigidbody2DComponent::BodyType>(comp->m_Type), comp->m_FixedRotation,
			nullptr, comp->m_LinearVelocity, comp->m_AngularVelocity, comp->m_LinearDamping, comp->m_AngularDamping,
			comp->m_GravityScale);

	}


	JSON PhysicsSystem::WriteRigidbody2DComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& rigidbody = *static_cast<Rigidbody2DComponent*>(component);

		json["Rigidbody2D"]["type"] = rigidbody.m_Type;
		json["Rigidbody2D"]["fixedRotation"] = rigidbody.m_FixedRotation;
		json["Rigidbody2D"]["linearVelocity"] = { rigidbody.m_LinearVelocity.x, rigidbody.m_LinearVelocity.y };
		json["Rigidbody2D"]["angularVelocity"] = rigidbody.m_AngularVelocity;
		json["Rigidbody2D"]["linearDamping"] = rigidbody.m_LinearDamping;
		json["Rigidbody2D"]["angularDamping"] = rigidbody.m_AngularDamping;
		json["Rigidbody2D"]["gravityScale"] = rigidbody.m_GravityScale;

		return json;
	}

	void PhysicsSystem::ViewRigidbody2DComponent(Entity& entity)
	{
		Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();
		if (rb2d.m_RuntimeBody)
		{
			if (ImGui::Checkbox("Fixed Rotation", &rb2d.m_FixedRotation))
				rb2d.m_RuntimeBody->SetFixedRotation(rb2d.m_FixedRotation);

			//ImGui::Text(magic_enum::enum_name(rb2d.m_Type).data()); ImGui::SameLine();

	/*		if (ImGui::DragInt("Body Type", reinterpret_cast<int*>(&(rb2d.m_Type)), 1.0f, 0, Rigidbody2DComponent::BodyType::BodyTypes - 1))

				if (rb2d.m_Type > Rigidbody2DComponent::BodyType::BodyTypes - 1)
				{
					rb2d.m_Type = (Rigidbody2DComponent::BodyType)static_cast<int>(Rigidbody2DComponent::BodyType::BodyTypes - 1);
				}

			rb2d.m_RuntimeBody->SetType(static_cast<b2BodyType>(rb2d.m_Type));*/


			if (ImGui::BeginCombo("Body Type", magic_enum::enum_name(rb2d.m_Type).data()))
			{
				for (size_t i = 0; i < Rigidbody2DComponent::BodyType::BodyTypes; i++)
				{
					bool isSelected = rb2d.m_Type == static_cast<Rigidbody2DComponent::BodyType>(i);

					if (ImGui::Selectable(magic_enum::enum_name(static_cast<Rigidbody2DComponent::BodyType>(i)).data(), isSelected)) {
						rb2d.m_Type = static_cast<Rigidbody2DComponent::BodyType>(i);
						rb2d.m_RuntimeBody->SetType(static_cast<b2BodyType>(rb2d.m_Type));
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus(); // Set focus on the selected item
					}
				}

				ImGui::EndCombo();
			}

			if (ImGui::DragFloat2("Linear Velocity", glm::value_ptr(rb2d.m_LinearVelocity), 0.5f))
			{
				// Handle the change in translation
				auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });

			}

			if (ImGui::DragFloat("Linear Damping", &rb2d.m_LinearDamping, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearDamping(rb2d.m_LinearDamping);
			}

			if (ImGui::DragFloat("Angular Velocity", &rb2d.m_AngularVelocity, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetAngularVelocity(rb2d.m_AngularVelocity);
			}

			if (ImGui::DragFloat("Angular Damping", &rb2d.m_AngularDamping, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearDamping(rb2d.m_AngularDamping);
			}

			if (ImGui::DragFloat("Gravity Scale", &rb2d.m_GravityScale, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetGravityScale(rb2d.m_GravityScale);
			}
		}
		else
		{
			// Display red warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Collider2D component");
		}

		//update body when data changes
	}

	void PhysicsSystem::LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation)
	{
		glm::vec3 eulerRadians = glm::radians(rotation);

		// Create a quaternion from Euler angles (yaw, pitch, roll)
		auto orientation = glm::quat(glm::yawPitchRoll(eulerRadians.y, eulerRadians.x, eulerRadians.z));

		glm::vec3 scale;
		glm::vec3 skew; // usually can be set to glm::vec3(0.0f)
		glm::vec4 perspective; // usually can be set to glm::vec4(0.0f)
		glm::decompose(transform.m_TransformMatrix, scale, orientation, translation, skew, perspective);


		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(orientation));
		rotation = localRotationEuler;
	}

	void PhysicsSystem::LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		glm::vec3 eulerRadians = glm::radians(rotation);

		// Create a quaternion from Euler angles (yaw, pitch, roll)
		auto orientation = glm::quat(glm::yawPitchRoll(eulerRadians.y, eulerRadians.x, eulerRadians.z));

		glm::vec3 skew; // usually can be set to glm::vec3(0.0f)
		glm::vec4 perspective; // usually can be set to glm::vec4(0.0f)
		glm::decompose(transform.m_TransformMatrix, scale, orientation, translation, skew, perspective);


		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(orientation));
		rotation = localRotationEuler;
	}

	void PhysicsSystem::WorldToLocalPos(TransformComponent& transform, TransformComponent& parentTransform)
	{
		glm::mat4 inverseParentTransform = glm::inverse(parentTransform.m_TransformMatrix);

		transform.m_Translation = glm::vec3((inverseParentTransform * glm::vec4(transform.m_Translation, 1.0f)));

		// Extract parent rotation as Euler angles (assume in degrees)
		glm::vec3 parentRotationEuler = parentTransform.m_Rotation; // Euler angles in degrees

		// Convert parent rotation to quaternion
		glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
		glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

		// Convert child rotation from Euler angles (assume in degrees)
		glm::vec3 childRotationEuler = transform.m_Rotation; // Euler angles in degrees
		glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
		glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

		// Compute local rotation by applying the inverse of the parent’s rotation
		glm::quat localRotation = glm::normalize(glm::inverse(parentRotation) * childRotation);

		// Convert local rotation back to Euler angles
		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));
		transform.m_Rotation = localRotationEuler;
	}

	//bool PhysicsSystem::HasRequiredComponents(NRegistry* registry, EntityID entityID)
	//{
	//	bool hasrb2d = registry->HasComponent<Rigidbody2DComponent>(entityID);
	//	bool hastransform = registry->HasComponent<TransformComponent>(entityID);
	//	bool hasColliders = (registry->HasComponent<BoxCollider2DComponent>(entityID) ||
	//		registry->HasComponent<CircleCollider2DComponent>(entityID));

	//	//if (!hasrb2d)
	//	//{
	//	//	if
	//	//}
	//}

	bool PhysicsSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			eventManager->QueueAsync(std::make_unique<InitializeBox2DEvent>(e.GetID()));


		return true;
	}

	bool PhysicsSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		if (e.GetComponentID() == Component<Rigidbody2DComponent>::GetID())
		{
			if (registry->HasComponent<Rigidbody2DComponent>(e.GetID()))
			{
				Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(e.GetID());

				if (rb2d.m_RuntimeBody)
				{
					if (registry->HasComponent<BoxCollider2DComponent>(e.GetID()))
					{
						BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(e.GetID());

						if (bc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);

							bc2d.m_RuntimeFixture = nullptr;
						}

					}

					if (registry->HasComponent<CircleCollider2DComponent>(e.GetID()))
					{
						CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(e.GetID());

						if (cc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(cc2d.m_RuntimeFixture);

							cc2d.m_RuntimeFixture = nullptr;
						}
					}

					m_PhysicsWorld->DestroyBody(rb2d.m_RuntimeBody);

					rb2d.m_RuntimeBody = nullptr;
				}

			}
		}
		return true;
	}

	bool PhysicsSystem::OnEntityParented(const EntityParentedEvent& e)
	{
		auto* sceneManager = NEngine::Instance().Get<NSceneManager>();

		auto& parent = sceneManager->GetCurrentScene()->GetEntity(e.GetParentID());
		auto& child = sceneManager->GetCurrentScene()->GetEntity(e.GetChildID());

		if (parent.Has<Rigidbody2DComponent>())
		{
			auto& rb2d = parent.Get<Rigidbody2DComponent>();

			if (!child.Has<Rigidbody2DComponent>())
			{
				if (child.Has<BoxCollider2DComponent>() ||
					child.Has<CircleCollider2DComponent>())
				{
					if (child.Has<BoxCollider2DComponent>())
					{
						BoxCollider2DComponent& bc2d = child.Get<BoxCollider2DComponent>();


						if (!bc2d.m_RuntimeFixture)
						{
							b2PolygonShape boxShape;

							auto scale = PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
							auto offset = PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

							boxShape.SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);

							b2FixtureDef fixDef;

							fixDef.shape = &boxShape;
							fixDef.density = bc2d.m_Density;
							fixDef.friction = bc2d.m_Friction;
							fixDef.restitution = bc2d.m_Restitution;
							fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

							bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
						}
					}

					if (child.Has<CircleCollider2DComponent>())
					{
						CircleCollider2DComponent& cc2d = child.Get<CircleCollider2DComponent>();

						if (!cc2d.m_RuntimeFixture)
						{
							b2CircleShape circleShape;

							auto offset = PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
							auto radius = PixelsToMeters(cc2d.m_Radius);


							circleShape.m_p.Set(offset.x, offset.y);
							circleShape.m_radius = radius;

							b2FixtureDef fixDef;

							fixDef.shape = &circleShape;
							fixDef.density = cc2d.m_Density;
							fixDef.friction = cc2d.m_Friction;
							fixDef.restitution = cc2d.m_Restitution;
							fixDef.restitutionThreshold = cc2d.m_RestitutionThreshold;

							cc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
						}
					}

				}
			}
		}
		else
		{
			if (parent.Has<ParentComponent>())
			{
				auto& pComp = parent.Get<ParentComponent>();

				auto& gParent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

				HandleParenting_Rec(sceneManager,gParent, child);
			}
		}

		return true;
	}

	bool PhysicsSystem::OnEntitySeparated(const EntitySeparatedEvent& e)
	{
		auto* sceneManager = NEngine::Instance().Get<NSceneManager>();

		auto& parent = sceneManager->GetCurrentScene()->GetEntity(e.GetParentID());
		auto& child = sceneManager->GetCurrentScene()->GetEntity(e.GetChildID());

		if (parent.Has<Rigidbody2DComponent>())
		{
			auto& rb2d = parent.Get<Rigidbody2DComponent>();

			if (!child.Has<Rigidbody2DComponent>())
			{
				if (child.Has<BoxCollider2DComponent>() ||
					child.Has<CircleCollider2DComponent>())
				{
					if (child.Has<BoxCollider2DComponent>())
					{
						BoxCollider2DComponent& bc2d = child.Get<BoxCollider2DComponent>();


						if (!bc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);
						}
					}

					if (child.Has<CircleCollider2DComponent>())
					{
						CircleCollider2DComponent& cc2d = child.Get<CircleCollider2DComponent>();

						if (cc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(cc2d.m_RuntimeFixture);
						}
					}

				}
			}
		}
		else
		{
			if (parent.Has<ParentComponent>())
			{
				auto& pComp = parent.Get<ParentComponent>();

				auto& gParent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

				HandleSeparation_Rec(sceneManager, gParent, child);
			}
		}

		return true;
	}

	bool PhysicsSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
		{
			if (e.GetComponentID() == Component<BoxCollider2DComponent>::GetID() ||
				e.GetComponentID() == Component<CircleCollider2DComponent>::GetID())
			{
				eventManager->QueueAsync(std::make_unique<InitializeBox2DEvent>(e.GetID()));
			}
		}

		return true;
	}

	bool PhysicsSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		for (size_t i = 0; i < m_Entities.size(); i++)
		{
			EntityID id = m_Entities[i];

			NRegistry* registry = NEngine::Instance().Get<NRegistry>();

			Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(id);

			if (registry->HasComponent<BoxCollider2DComponent>(id))
			{
				BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(id);

				if (bc2d.m_RuntimeFixture)
				{
					rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);

					bc2d.m_RuntimeFixture = nullptr;
				}

			}


			if (registry->HasComponent<CircleCollider2DComponent>(id))
			{
				CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(id);

				if (cc2d.m_RuntimeFixture)
				{
					rb2d.m_RuntimeBody->DestroyFixture(cc2d.m_RuntimeFixture);

					cc2d.m_RuntimeFixture = nullptr;
				}

			}

			if (rb2d.m_RuntimeBody)
			{
				m_PhysicsWorld->DestroyBody(rb2d.m_RuntimeBody);

				rb2d.m_RuntimeBody = nullptr;
			}
		}

		return true;
	}

	bool PhysicsSystem::OnSceneStart(const InitializeBox2DEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		return InitializePhysics(e.GetEntityID(), registry);
	}

	void PhysicsSystem::HandleParenting_Rec(NSceneManager* sceneManager,Entity& parent, Entity& child)
	{
		if (parent.Has<Rigidbody2DComponent>())
		{
			auto& rb2d = parent.Get<Rigidbody2DComponent>();

			if (!child.Has<Rigidbody2DComponent>())
			{
				if (child.Has<BoxCollider2DComponent>() ||
					child.Has<CircleCollider2DComponent>())
				{
					if (child.Has<BoxCollider2DComponent>())
					{
						BoxCollider2DComponent& bc2d = child.Get<BoxCollider2DComponent>();


						if (!bc2d.m_RuntimeFixture)
						{
							b2PolygonShape boxShape;

							auto scale = PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
							auto offset = PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

							boxShape.SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);

							b2FixtureDef fixDef;

							fixDef.shape = &boxShape;
							fixDef.density = bc2d.m_Density;
							fixDef.friction = bc2d.m_Friction;
							fixDef.restitution = bc2d.m_Restitution;
							fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

							bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
						}
					}

					if (child.Has<CircleCollider2DComponent>())
					{
						CircleCollider2DComponent& cc2d = child.Get<CircleCollider2DComponent>();

						if (!cc2d.m_RuntimeFixture)
						{
							b2CircleShape circleShape;

							auto offset = PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
							auto radius = PixelsToMeters(cc2d.m_Radius);


							circleShape.m_p.Set(offset.x, offset.y);
							circleShape.m_radius = radius;

							b2FixtureDef fixDef;

							fixDef.shape = &circleShape;
							fixDef.density = cc2d.m_Density;
							fixDef.friction = cc2d.m_Friction;
							fixDef.restitution = cc2d.m_Restitution;
							fixDef.restitutionThreshold = cc2d.m_RestitutionThreshold;

							cc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
						}
					}

				}
			}
		}
		else
		{
			if (parent.Has<ParentComponent>())
			{
				auto& pComp = parent.Get<ParentComponent>();

				auto& gParent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

				HandleParenting_Rec(sceneManager,gParent, child);
			}
		}
	}

	void PhysicsSystem::HandleSeparation_Rec(NSceneManager* sceneManager, Entity& parent, Entity& child)
	{
		if (parent.Has<Rigidbody2DComponent>())
		{
			auto& rb2d = parent.Get<Rigidbody2DComponent>();

			if (!child.Has<Rigidbody2DComponent>())
			{
				if (child.Has<BoxCollider2DComponent>() ||
					child.Has<CircleCollider2DComponent>())
				{
					if (child.Has<BoxCollider2DComponent>())
					{
						BoxCollider2DComponent& bc2d = child.Get<BoxCollider2DComponent>();


						if (!bc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);
						}
					}

					if (child.Has<CircleCollider2DComponent>())
					{
						CircleCollider2DComponent& cc2d = child.Get<CircleCollider2DComponent>();

						if (cc2d.m_RuntimeFixture)
						{
							rb2d.m_RuntimeBody->DestroyFixture(cc2d.m_RuntimeFixture);
						}
					}

				}
			}
		}
		else
		{
			if (parent.Has<ParentComponent>())
			{
				auto& pComp = parent.Get<ParentComponent>();

				auto& gParent = sceneManager->GetCurrentScene()->GetEntity(pComp.m_Parent);

				HandleSeparation_Rec(sceneManager, gParent, child);
			}
		}
	}

	bool PhysicsSystem::InitializePhysics(EntityID entityId, NRegistry* registry)
	{
		TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);
		Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(entityId);
		if (!rb2d.m_RuntimeBody)
		{
			b2BodyDef bodyDef;

			bodyDef.type = (b2BodyType)rb2d.m_Type;

			glm::vec3 translation = transform.m_Translation;
			glm::vec3 rotation = transform.m_Rotation;


			if (registry->HasComponent<ParentComponent>(entityId))
			{
				auto& parentComp = registry->GetComponent<ParentComponent>(entityId);

				TransformComponent& parentTransform = registry->GetComponent<TransformComponent>(parentComp.m_Parent);

				if (parentTransform.m_Dirty)
				{
					return false;
				}

				LocalToWorldPos(transform, translation, rotation);
			}

			auto pos = PixelsToMeters(translation.x, translation.y);
			auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

			bodyDef.position.Set(pos.x, pos.y);
			bodyDef.linearVelocity.Set(vel.x, vel.y);
			bodyDef.angularVelocity = rb2d.m_AngularVelocity;
			bodyDef.angularDamping = rb2d.m_AngularDamping;
			bodyDef.linearDamping = rb2d.m_LinearDamping;
			bodyDef.gravityScale = rb2d.m_GravityScale;
			bodyDef.angle = rotation.z;


			rb2d.m_RuntimeBody = m_PhysicsWorld->CreateBody(&bodyDef);

			rb2d.m_RuntimeBody->SetFixedRotation(rb2d.m_FixedRotation);
		}

		if (rb2d.m_RuntimeBody)
			return InitializeColliders(entityId, registry, rb2d);

		return true;
	}

	bool PhysicsSystem::InitializeColliders(EntityID entityID, NRegistry* registry, Rigidbody2DComponent& rb2d)
	{
		if (registry->HasComponent<BoxCollider2DComponent>(entityID))
		{
			BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(entityID);


			if (!bc2d.m_RuntimeFixture)
			{
				b2PolygonShape boxShape;

				auto scale = PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
				auto offset = PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

				boxShape.SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);

				b2FixtureDef fixDef;

				fixDef.shape = &boxShape;
				fixDef.density = bc2d.m_Density;
				fixDef.friction = bc2d.m_Friction;
				fixDef.restitution = bc2d.m_Restitution;
				fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

				bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
			}
		}

		if (registry->HasComponent<CircleCollider2DComponent>(entityID))
		{
			CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(entityID);

			if (!cc2d.m_RuntimeFixture)
			{
				b2CircleShape circleShape;

				auto offset = PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
				auto radius = PixelsToMeters(cc2d.m_Radius);


				circleShape.m_p.Set(offset.x, offset.y);
				circleShape.m_radius = radius;

				b2FixtureDef fixDef;

				fixDef.shape = &circleShape;
				fixDef.density = cc2d.m_Density;
				fixDef.friction = cc2d.m_Friction;
				fixDef.restitution = cc2d.m_Restitution;
				fixDef.restitutionThreshold = cc2d.m_RestitutionThreshold;

				cc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
			}
		}

		if (registry->HasComponent<ChildrenComponent>(entityID))
		{
			auto transform = registry->GetComponent<TransformComponent>(entityID);

			if (transform.m_Dirty)
				return false;

			auto& cComp = registry->GetComponent<ChildrenComponent>(entityID);

			auto& children = cComp.m_Children;

			bool ret = true;
			for (auto& child : children)
			{
				if (registry->HasComponent<Rigidbody2DComponent>(child))
					continue;
				
				ret = InitializeChildrenColliders(child, registry, rb2d, transform);
			}

			return ret;
		}

		return true;
	}

	bool PhysicsSystem::InitializeChildrenColliders(EntityID entityID, NRegistry* registry, Rigidbody2DComponent& rb2d, TransformComponent& parentTransform)
	{
		TransformComponent& transform = registry->GetComponent<TransformComponent>(entityID);

		auto rotation = transform.m_Rotation;
		auto translation = transform.m_Translation;

		LocalToWorldPos(transform, translation, rotation);

		if (registry->HasComponent<BoxCollider2DComponent>(entityID))
		{
			BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(entityID);

			if (!bc2d.m_RuntimeFixture)
			{
				b2PolygonShape boxShape;
				auto childOffset = translation - parentTransform.m_Translation;
				auto scale = PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
				auto childWorldPositionMeters = PixelsToMeters(childOffset.x, childOffset.y);

				// Convert collider’s local offset to meters
				auto colliderOffsetMeters = PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

				// Combine the child’s world position and the collider’s local offset
				b2Vec2 finalOffset(childWorldPositionMeters.x + colliderOffsetMeters.x,
					childWorldPositionMeters.y + colliderOffsetMeters.y);

				boxShape.SetAsBox(scale.x, scale.y, b2Vec2(finalOffset.x, finalOffset.y), 0.0f);

				b2FixtureDef fixDef;

				fixDef.shape = &boxShape;
				fixDef.density = bc2d.m_Density;
				fixDef.friction = bc2d.m_Friction;
				fixDef.restitution = bc2d.m_Restitution;
				fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

				bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
			}
		}

		if (registry->HasComponent<CircleCollider2DComponent>(entityID))
		{
			auto transform = registry->GetComponent<TransformComponent>(entityID);

			if (transform.m_Dirty)
				return false;

			CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(entityID);

			if (!cc2d.m_RuntimeFixture)
			{
				b2CircleShape circleShape;

				auto radius = PixelsToMeters(cc2d.m_Radius);
				auto childOffset = translation - parentTransform.m_Translation;

				auto childWorldPositionMeters = PixelsToMeters(childOffset.x, childOffset.y);

				// Convert collider’s local offset to meters
				auto colliderOffsetMeters = PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);

				// Combine the child’s world position and the collider’s local offset
				b2Vec2 finalOffset(childWorldPositionMeters.x + colliderOffsetMeters.x,
					childWorldPositionMeters.y + colliderOffsetMeters.y);

				circleShape.m_p.Set(finalOffset.x, finalOffset.y);
				circleShape.m_radius = radius;

				b2FixtureDef fixDef;

				fixDef.shape = &circleShape;
				fixDef.density = cc2d.m_Density;
				fixDef.friction = cc2d.m_Friction;
				fixDef.restitution = cc2d.m_Restitution;
				fixDef.restitutionThreshold = cc2d.m_RestitutionThreshold;

				cc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
			}
		}


		if (registry->HasComponent<ChildrenComponent>(entityID))
		{
			auto& cComp = registry->GetComponent<ChildrenComponent>(entityID);

			auto& children = cComp.m_Children;

			bool ret = true;
			for (auto& child : children)
			{
				if (registry->HasComponent<Rigidbody2DComponent>(child))
					continue;

				ret = InitializeChildrenColliders(child, registry, rb2d, parentTransform);
			}

			return ret;
		}

		return true;
	}

}