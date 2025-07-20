
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
#include <box2d/box2d.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>
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
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"
#include "../../Scene.h"
#include "../../../../Tools/ImGuiH.h"
//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{



	class ContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override
		{
			NEventManager* eventManager = NEventManager::Instance();
			NSceneManager* sceneManager = NSceneManager::Instance();
			Scene* scene = sceneManager->GetCurrentScene();

			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			EntityID userDataA = static_cast<EntityID>(bodyA->GetUserData().pointer);
			EntityID userDataB = static_cast<EntityID>(bodyB->GetUserData().pointer);

			eventManager->TriggerEvent(CollisionEnterEvent(userDataA, userDataB));
			eventManager->TriggerEvent(CollisionEnterEvent(userDataB, userDataA));
			//NLE_CORE_DEBUG("{0} hit {1}", userDataA->m_Name, userDataB->m_Name);
		}
		void EndContact(b2Contact* contact) override
		{
			//NLE_CORE_DEBUG("Collision ended");
		}
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
		{
			//NLE_CORE_DEBUG("What to do");
		}
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
		{
			//NLE_CORE_DEBUG("It is done");
		}
	};


	static ContactListener s_ContactListener;
	float PhysicsSystem::m_Pixels_Per_Meter = 64.0f;



	b2BodyDef ToBodyDef(const Rigidbody2DComponent& rb, const glm::vec2& pos, const glm::vec2& vel, float angle)
	{
		b2BodyDef def;
		def.type = static_cast<b2BodyType>(rb.type);
		def.position.Set(pos.x, pos.y);
		def.angle = angle;
		def.fixedRotation = rb.fixedRotation;
		def.linearVelocity = { vel.x, vel.y };
		def.angularVelocity = rb.angularVelocity;
		def.linearDamping = rb.linearDamping;
		def.angularDamping = rb.angularDamping;
		def.gravityScale = rb.gravityScale;
		def.awake = true;
		def.allowSleep = true;
		return def;
	}



	PhysicsSystem::PhysicsSystem() : m_PhysicsWorld(nullptr)
	{
		Require<TransformComponent>();
		Require<Rigidbody2DComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<Rigidbody2DComponent>(CreateRigidbody2DComponent,
			[this](Entity& id) { this->ViewRigidbody2DComponent(id); }, WriteRigidbody2DComponent,
			AddRigidbody2DComponent, DiffRigidbody2DComponent);
	}
	void PhysicsSystem::Load()
	{
		auto gravity = PixelsToMeters(GRAVITY.x, GRAVITY.y);
		m_PhysicsWorld = new b2World({ gravity.x, gravity.y });
		m_PhysicsWorld->SetContactListener(&s_ContactListener);
	}

	void PhysicsSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEventManager::Instance();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &PhysicsSystem::OnEntityCreated, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityDestroyedEvent, &PhysicsSystem::OnEntityDestroyed, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &PhysicsSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &PhysicsSystem::OnEntityComponentAdded, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &PhysicsSystem::OnSceneSwitched, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(InitializeBox2DEvent, &PhysicsSystem::OnSceneStart, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntityParentedEvent, &PhysicsSystem::OnEntityParented, eventManager, EventPriority::High);
		//SUBSCRIBE_EVENT(EntitySeparatedEvent, &PhysicsSystem::OnEntitySeparated, eventManager, EventPriority::High);

		NRegistry* registry = NRegistry::Instance();


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
		NRegistry* m_Parent = NRegistry::Instance();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);


			if (!transform.flags.IsSet(TransformFlags_DirectManipulation))
				continue;

			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);

			if (body)
			{

				auto rotation = transform.rotation;
				auto translation = transform.translation;

				if (m_Parent->HasComponent<ParentComponent>(entityId))
				{
					//auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

					//TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);

					LocalToWorldPos(transform, translation, rotation);
				}


				auto pos = PixelsToMeters(translation.x, translation.y);
				body->SetTransform({ pos.x, pos.y }, rotation.z);

				transform.flags.Clear(TransformFlags_DirectManipulation);
			}

		}


		m_PhysicsWorld->Step(dt, velocityIterations, positionIterations);

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);

			if (body)
			{

				body->SetAwake(rb2d.componentFlags.IsSet(ComponentFlags_Enabled));

				if (!rb2d.componentFlags.IsSet(ComponentFlags_Enabled))
					continue;

				const auto& position = body->GetPosition();

				auto newPos = MetersToPixels(position.x, position.y);

				auto linearVel = body->GetLinearVelocity();

				rb2d.linearVelocity = MetersToPixels(linearVel.x, linearVel.y);

				transform.translation.x = newPos.x;
				transform.translation.y = newPos.y;

				transform.rotation.z = body->GetAngle();

				if (m_Parent->HasComponent<ParentComponent>(entityId))
				{
					auto& parentComp = m_Parent->GetComponent<ParentComponent>(entityId);

					TransformComponent& parentTransform = m_Parent->GetComponent<TransformComponent>(parentComp.m_Parent);
					WorldToLocalPos(transform, parentTransform);
				}


				transform.flags.Set(TransformFlags_Dirty);
			}
		}
	}

	void PhysicsSystem::Render()
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
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);
			b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);

			if (!body)
				continue;

			// Define a maximum length for the line
			float maxLength = 250.0f; // Example max length; adjust as needed

			// Calculate the current length of the velocity vector
			float velocityLength = glm::length(rb2d.linearVelocity);

			// Define a reference speed for scaling
			float referenceSpeed = 1000.0f; // Example reference speed; adjust as needed

			// Calculate the scaling factor based on the ratio of velocity to the reference speed
			float scaleFactor = glm::min(velocityLength / referenceSpeed, 1.0f);

			// Determine the actual length of the line based on the scale factor and maximum length
			float lineLength = maxLength * scaleFactor;

			// Normalize the velocity vector and calculate the offset
			auto offset = glm::normalize(rb2d.linearVelocity) * (lineLength / 2.0f);

			// Calculate the translation with the correct depth
			//auto translation = glm::vec3(transform.m_Translation.x, transform.m_Translation.y, (transform.m_Translation.z + transform.m_Scale.z * 0.5f + 0.50f));

			auto translation = camManager->GetCurrentCamera()->GetCameraType() == Camera::PERSPECTIVE ?
				(transform.translation + glm::vec3(offset, transform.translation.z + transform.scale.z + 0.50f)) :
				(transform.translation + glm::vec3(offset, transform.translation.z + 0.50f));

			// Calculate the angle of rotation based on the linear velocity vector
			float rot = glm::atan(rb2d.linearVelocity.y, rb2d.linearVelocity.x);

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

		ImGui::ColorEdit4("Vector Color", glm::value_ptr(m_Color));
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
				"linear_velocity", sol::readonly(&Rigidbody2DComponent::linearVelocity),
				"angular_velocity", sol::readonly(&Rigidbody2DComponent::angularVelocity),
				"angular_damping", sol::readonly(&Rigidbody2DComponent::angularDamping),
				"linear_damping", sol::readonly(&Rigidbody2DComponent::linearDamping),
				"gravity_scale", sol::readonly(&Rigidbody2DComponent::gravityScale),
				"set_linear_velocity", sol::overload(
					[this](Rigidbody2DComponent& rb2d, float x, float y)
					{
						rb2d.linearVelocity = glm::vec2(x, y);

						auto vel = PixelsToMeters(rb2d.linearVelocity.x, rb2d.linearVelocity.y);
						b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);
						if (body)
							body->SetLinearVelocity({ vel.x, vel.y });
					},
					[this](Rigidbody2DComponent& rb2d, glm::vec2 newVel)
					{
						rb2d.linearVelocity = newVel;

						auto vel = PixelsToMeters(rb2d.linearVelocity.x, rb2d.linearVelocity.y);
						b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);
						if (body)
							body->SetLinearVelocity({ vel.x, vel.y });
					}
				),
				"set_angular_velocity", [this](Rigidbody2DComponent& rb2d, float v)
				{
					rb2d.angularVelocity = v;
					b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);
					if (body)
						body->SetAngularVelocity(rb2d.angularVelocity);
				},
				"set_gravity_scale", [this](Rigidbody2DComponent& rb2d, float g)
				{
					rb2d.gravityScale = g;
					b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);
					if (body)
						body->SetGravityScale(rb2d.gravityScale);
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




	//uint32_t PhysicsSystem::CreateBody(const Rigidbody2DComponent& rb2d, EntityID entID)
	//{
	//	TransformComponent& transform = NRegistry::Instance()->GetComponent<TransformComponent>(entID);
	//	b2BodyDef bodyDef = ToBodyDef(rb2d, glm::vec2(transform.translation.x, transform.translation.y), transform.rotation.z);
	//}

	void PhysicsSystem::CreateRigidbody2DComponent(void* component, const nlohmann::json& json)
	{

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<Rigidbody2DComponent*>(component);
		JsonReader jsonWrapper(json);


		if (!jsonWrapper.Empty())
		{
			comp->type = static_cast<BodyType>(jsonWrapper.GetInt("type", 0));
			comp->fixedRotation = jsonWrapper.GetBool("fixedRotation", true);
			comp->linearVelocity = jsonWrapper.GetVec2("linearVelocity", glm::vec2(0, 0));
			comp->angularVelocity = jsonWrapper.GetFloat("angularVelocity", 0.0f);
			comp->linearDamping = jsonWrapper.GetFloat("linearDamping", 0.0f);
			comp->angularDamping = jsonWrapper.GetFloat("angularDamping", 0.0f);
			comp->gravityScale = jsonWrapper.GetFloat("gravityScale", 1.0f);
			comp->mass = jsonWrapper.GetFloat("mass", 1.0f);


			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

		}

		comp->runtimeBodyIndex = -1; // Initialize runtime index, will be set during initialization

	}


	void PhysicsSystem::AddRigidbody2DComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<Rigidbody2DComponent*>(component);

		componentFactory->AddOrUpdate<Rigidbody2DComponent>(
			id,
			comp,
			registry,
			comp->linearVelocity,
			comp->type,
			comp->angularVelocity,
			comp->linearDamping,
			comp->angularDamping,
			comp->mass,
			comp->gravityScale,
			comp->runtimeBodyIndex, // runtime index, will be set during initialization
			comp->fixedRotation,
			comp->componentFlags
		);
	}


	JSON PhysicsSystem::WriteRigidbody2DComponent(const void* component)
	{
		nlohmann::json json;

		const auto& rb = *static_cast<const Rigidbody2DComponent*>(component);

		nlohmann::json& out = json["Rigidbody2D"];

		out["type"] = rb.type;
		out["fixedRotation"] = rb.fixedRotation;
		out["linearVelocity"] = { rb.linearVelocity.x, rb.linearVelocity.y };
		out["angularVelocity"] = rb.angularVelocity;
		out["linearDamping"] = rb.linearDamping;
		out["angularDamping"] = rb.angularDamping;
		out["gravityScale"] = rb.gravityScale;
		out["mass"] = rb.mass;

		// Optional: write flags (if you serialize them)
		out["flags"] = rb.componentFlags.m_Flags;

		return json;
	}


	JSON PhysicsSystem::DiffRigidbody2DComponent(const void* base, const void* modified)
	{
		JSON diff;

		const auto& a = *static_cast<const Rigidbody2DComponent*>(base);
		const auto& b = *static_cast<const Rigidbody2DComponent*>(modified);

		if (a.type != b.type)
			diff["type"] = b.type;

		if (a.fixedRotation != b.fixedRotation)
			diff["fixedRotation"] = b.fixedRotation;

		if (a.linearVelocity.x != b.linearVelocity.x || a.linearVelocity.y != b.linearVelocity.y)
			diff["linearVelocity"] = { b.linearVelocity.x, b.linearVelocity.y };

		if (a.angularVelocity != b.angularVelocity)
			diff["angularVelocity"] = b.angularVelocity;

		if (a.linearDamping != b.linearDamping)
			diff["linearDamping"] = b.linearDamping;

		if (a.angularDamping != b.angularDamping)
			diff["angularDamping"] = b.angularDamping;

		if (a.mass != b.mass)
			diff["mass"] = b.mass;

		if (a.gravityScale != b.gravityScale)
			diff["gravityScale"] = b.gravityScale;

		if (a.componentFlags.m_Flags != b.componentFlags.m_Flags)
			diff["flags"] = b.componentFlags.m_Flags;

		return diff;
	}


	void PhysicsSystem::ViewRigidbody2DComponent(Entity& entity)
	{
			Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();
			uint8_t& flags = rb2d.componentFlags.m_Flags;
			// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
			auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Rigidbody2D", flags, ComponentFlags_Enabled);

			if (remove)
			{
				NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<Rigidbody2DComponent>::GetID()));
			}

			if (!open)
				return;

			if (!enabled)
				ImGui::BeginDisabled();

			b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);


			if (body)
			{
				if (ImGui::Checkbox("Fixed Rotation", &rb2d.fixedRotation))
					body->SetFixedRotation(rb2d.fixedRotation);



				if (ImGui::BeginCombo("Body Type", magic_enum::enum_name(rb2d.type).data()))
				{
					for (size_t i = 0; i < BodyType::BodyTypes; i++)
					{
						bool isSelected = rb2d.type == static_cast<BodyType>(i);

						if (ImGui::Selectable(magic_enum::enum_name(static_cast<BodyType>(i)).data(), isSelected)) {
							rb2d.type = static_cast<BodyType>(i);
							body->SetType(static_cast<b2BodyType>(rb2d.type));
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus(); // Set focus on the selected item
						}
					}

					ImGui::EndCombo();
				}

				if (ImGui::DragFloat2("Linear Velocity", glm::value_ptr(rb2d.linearVelocity), 0.5f))
				{
					// Handle the change in translation
					auto vel = PixelsToMeters(rb2d.linearVelocity.x, rb2d.linearVelocity.y);

					if (body)
						body->SetLinearVelocity({ vel.x, vel.y });

				}

				if (ImGui::DragFloat("Linear Damping", &rb2d.linearDamping, 0.5f))
				{
					if (body)
						body->SetLinearDamping(rb2d.linearDamping);
				}

				if (ImGui::DragFloat("Angular Velocity", &rb2d.angularVelocity, 0.5f))
				{
					if (body)
						body->SetAngularVelocity(rb2d.angularVelocity);
				}

				if (ImGui::DragFloat("Angular Damping", &rb2d.angularDamping, 0.5f))
				{
					if (body)
						body->SetLinearDamping(rb2d.angularDamping);
				}

				if (!entity.Has<BoxCollider2DComponent>() && !entity.Has<CircleCollider2DComponent>())
				{
					if (ImGui::DragFloat("Mass", &rb2d.mass, 0.5f))
					{

						if (body)
						{
							TransformComponent& transform = entity.Get<TransformComponent>();
							auto pos = PixelsToMeters(transform.translation.x, transform.translation.y);
						

							b2MassData md;
							md.mass = rb2d.mass;
							md.center = { pos.x, pos.y };
							body->ResetMassData();
							body->SetMassData(&md);
						}
					}
				}

				if (ImGui::DragFloat("Gravity Scale", &rb2d.gravityScale, 0.5f))
				{
					if (body)
						body->SetGravityScale(rb2d.gravityScale);
				}
			}
			else
			{
				// Display red warning message
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Collider2D component");
			}

			if (!enabled)
				ImGui::EndDisabled();


			ImGui::TreePop();
	}

	void PhysicsSystem::LocalToWorldPos(TransformComponent& transform, glm::vec3& translation, glm::vec3& rotation)
	{
		glm::vec3 eulerRadians = glm::radians(rotation);

		// Create a quaternion from Euler angles (yaw, pitch, roll)
		auto orientation = glm::quat(glm::yawPitchRoll(eulerRadians.y, eulerRadians.x, eulerRadians.z));

		glm::vec3 scale;
		glm::vec3 skew; // usually can be set to glm::vec3(0.0f)
		glm::vec4 perspective; // usually can be set to glm::vec4(0.0f)
		glm::decompose(transform.transformMatrix, scale, orientation, translation, skew, perspective);


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
		glm::decompose(transform.transformMatrix, scale, orientation, translation, skew, perspective);


		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(orientation));
		rotation = localRotationEuler;
	}

	void PhysicsSystem::WorldToLocalPos(TransformComponent& transform, TransformComponent& parentTransform)
	{
		glm::mat4 inverseParentTransform = glm::inverse(parentTransform.transformMatrix);

		transform.translation = glm::vec3((inverseParentTransform * glm::vec4(transform.translation, 1.0f)));

		// Extract parent rotation as Euler angles (assume in degrees)
		glm::vec3 parentRotationEuler = parentTransform.rotation; // Euler angles in degrees

		// Convert parent rotation to quaternion
		glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
		glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

		// Convert child rotation from Euler angles (assume in degrees)
		glm::vec3 childRotationEuler = transform.rotation; // Euler angles in degrees
		glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
		glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

		// Compute local rotation by applying the inverse of the parent’s rotation
		glm::quat localRotation = glm::normalize(glm::inverse(parentRotation) * childRotation);

		// Convert local rotation back to Euler angles
		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));
		transform.rotation = localRotationEuler;
	}

	size_t PhysicsSystem::AddActiveBody(b2Body* body)
	{
		size_t index = m_ActiveBodies.size();
		if (!m_FreeBodyIDs.empty())
		{
			index = m_FreeBodyIDs.back();
			m_FreeBodyIDs.pop_back();
			m_ActiveBodies[index] = body;
		}
		else
		{
			m_ActiveBodies.push_back(body);
		}

		return index;
	}

	size_t PhysicsSystem::AddActiveFixture(b2Fixture* fixture)
	{
		size_t index = m_ActiveFixtures.size();
		if (!m_FreeFixtureIDs.empty())
		{
			index = m_FreeFixtureIDs.back();
			m_FreeFixtureIDs.pop_back();
			m_ActiveFixtures[index] = fixture;
		}
		else
		{
			m_ActiveFixtures.push_back(fixture);
		}

		return index;
	}

	void PhysicsSystem::RemoveActiveBody(uint32_t index)
	{

		if (!IsValidRuntimeIndex(index))
		{
			return;
		}


		if (index >= m_ActiveBodies.size())
		{
			NLE_CORE_ERROR("Index out of bounds: {0} (size: {1})", index, m_ActiveBodies.size());
			return;
		}


		if (m_ActiveBodies[index] == nullptr)
		{
			NLE_CORE_WARN("Attempted to remove body at index {0}, but slot is already empty.", index);
			return;
		}

		m_PhysicsWorld->DestroyBody(m_ActiveBodies[index]); // Destroy the Box2D body	

		m_ActiveBodies[index] = nullptr; // Mark the body as removed
		m_FreeBodyIDs.push_back(index); // Add the index to the free list
	}


	void PhysicsSystem::RemoveActiveFixture(uint32_t index, b2Body* body)
	{

		if (index >= m_ActiveFixtures.size())
		{
			NLE_CORE_ERROR("Index out of bounds: {0} (size: {1})", index, m_ActiveFixtures.size());
			return;
		}


		if (!IsValidRuntimeIndex(index))
		{
			NLE_CORE_ERROR("Invalid runtime index: {0}", index);

			return;
		}

		if (m_ActiveFixtures[index] == nullptr)
		{
			NLE_CORE_WARN("Attempted to remove body at index {0}, but slot is already empty.", index);
			return;
		}



		body->DestroyFixture(m_ActiveFixtures[index]); // Destroy the Box2D fixture	

		m_ActiveFixtures[index] = nullptr; // Mark the body as removed
		m_FreeFixtureIDs.push_back(index); // Add the index to the free list
	}

	b2Body* PhysicsSystem::GetActiveBody(uint32_t index) const
	{
		if(!IsValidRuntimeIndex(index))
		{
			NLE_CORE_ERROR("Invalid runtime index: {0}", index);
			return nullptr;
		}

		if (index >= m_ActiveBodies.size())
		{
			NLE_CORE_ERROR("Index out of bounds: {0} (size: {1})", index, m_ActiveBodies.size());
			return nullptr;
		}

		if (m_ActiveBodies[index] == nullptr)
		{
			NLE_CORE_WARN("Attempted to access body at index {0}, but slot is empty.", index);
			return nullptr;
		}

		return m_ActiveBodies[index];
	}

	b2Fixture* PhysicsSystem::GetActiveFixture(uint32_t index) const
	{
		if (!IsValidRuntimeIndex(index))
		{
			NLE_CORE_ERROR("Invalid runtime index: {0}", index);
			return nullptr;
		}

		if (index >= m_ActiveFixtures.size())
			{
				NLE_CORE_ERROR("Index out of bounds: {0} (size: {1})", index, m_ActiveFixtures.size());
				return nullptr;
		}

		if (m_ActiveFixtures[index] == nullptr)
		{
			NLE_CORE_WARN("Attempted to access fixture at index {0}, but slot is empty.", index);
			return nullptr;
		}

		return m_ActiveFixtures[index];
	}

	b2Body* PhysicsSystem::CreateBody(const b2BodyDef& bodyDef)
	{
		return m_PhysicsWorld->CreateBody(&bodyDef);
	}

	b2Fixture* PhysicsSystem::CreateFixture(b2Body* body, const b2FixtureDef& fixtureDef)
	{
		return body->CreateFixture(&fixtureDef);
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
		NEventManager* eventManager = NEventManager::Instance();
		NRegistry* registry = NRegistry::Instance();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			eventManager->QueueAsync(std::make_unique<InitializeBox2DEvent>(e.GetID()));


		return true;
	}

	bool PhysicsSystem::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		NEventManager* eventManager = NEventManager::Instance();
		NRegistry* registry = NRegistry::Instance();

		const auto& entityList = GetSystemEntities();

		// if entity is not in the system, we don't need to do anything
		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
		{
			// if it has a rigidbody component, we need to remove the body
			if (registry->HasComponent<Rigidbody2DComponent>(e.GetID()))
			{
				Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(e.GetID());

				b2Body* body = GetActiveBody(rb2d.runtimeBodyIndex);

				// if body is valid, we need to remove it as well as all its fixtures
				if (body)
				{

					if (registry->HasComponent<BoxCollider2DComponent>(e.GetID()))
					{
						BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(e.GetID());

						for (size_t i = 0; i < bc2d.colliderCount; i++)
						{
							b2Fixture* fixture =  GetActiveFixture(bc2d.colliders[i].runtimeFixtureIndex);

							if (fixture)
							{
								RemoveActiveFixture(bc2d.colliders[i].runtimeFixtureIndex, body);
							}
						}

					}

					if (registry->HasComponent<CircleCollider2DComponent>(e.GetID()))
					{
						CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(e.GetID());


						for (size_t i = 0; i < cc2d.colliderCount; i++)
						{
							b2Fixture* fixture = GetActiveFixture(cc2d.colliders[i].runtimeFixtureIndex);

							if (fixture)
							{
								RemoveActiveFixture(cc2d.colliders[i].runtimeFixtureIndex, body);
							}
						}

					}

				}

			}
		}

		//ISystem::OnEntityDestroyed(e);
		return true;
	}

	bool PhysicsSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		//if its a rigidbody component, we need to remove the body
		if (e.GetComponentID() == Component<Rigidbody2DComponent>::GetID())
		{
			//but only if it does not have any collider component
			if (registry->HasComponent<BoxCollider2DComponent>(e.GetID()) || registry->HasComponent<CircleCollider2DComponent>(e.GetID()))
				return true;

			Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(e.GetID());

			RemoveActiveBody(rb2d.runtimeBodyIndex);
		}

		return true;
	}



	bool PhysicsSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
		{
			// if the entity is not in the system, we need to add it
			if (e.GetComponentID() == Component<Rigidbody2DComponent>::GetID())
			{
				eventManager->QueueAsync(std::make_unique<InitializeBox2DEvent>(e.GetID()));
			}
		}


		return true;
	}

	bool PhysicsSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		// Clear all active bodies and fixtures in the current scene
		for (auto* body : m_ActiveBodies)
		{
			if (body != nullptr)
			{
				auto* fixtureList = body->GetFixtureList();

				while (fixtureList != nullptr)
				{
					auto* nextFixture = fixtureList->GetNext();
					// Destroy the fixture
					body->DestroyFixture(fixtureList);
					fixtureList = nextFixture; // Move to the next fixture
				}
				// Destroy all bodies in the current scene
				m_PhysicsWorld->DestroyBody(body);
				body = nullptr; // Set to nullptr to avoid dangling pointer
			}
		}

		m_ActiveBodies.clear();
		m_FreeBodyIDs.clear();
		m_ActiveFixtures.clear();
		m_FreeFixtureIDs.clear();


		return true;
	}

	bool PhysicsSystem::OnSceneStart(const InitializeBox2DEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		return InitializePhysics(e.GetEntityID(), registry);
	}

	bool PhysicsSystem::InitializePhysics(EntityID entityId, NRegistry* registry)
	{
		auto* sceneManager = NSceneManager::Instance();

		TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);
		Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(entityId);
		if (!IsValidRuntimeIndex(rb2d.runtimeBodyIndex))
		{


			glm::vec3 translation = transform.translation;
			glm::vec3 rotation = transform.rotation;


			if (registry->HasComponent<ParentComponent>(entityId))
			{
				auto& parentComp = registry->GetComponent<ParentComponent>(entityId);

				TransformComponent& parentTransform = registry->GetComponent<TransformComponent>(parentComp.m_Parent);

				if (parentTransform.flags.IsSet(TransformFlags_Dirty))
				{
					return false;
				}

				LocalToWorldPos(transform, translation, rotation);
			}

			auto pos = PixelsToMeters(translation.x, translation.y);
			auto vel = PixelsToMeters(rb2d.linearVelocity.x, rb2d.linearVelocity.y);


			b2BodyDef bodyDef = ToBodyDef(rb2d, pos, vel, rotation.z);

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

			b2MassData md;
			md.mass = rb2d.mass;
			md.center = { pos.x, pos.y };
			body->SetMassData(&md);

			auto& entity = sceneManager->GetCurrentScene()->GetEntity(entityId);
			body->GetUserData().pointer = entity.GetID();

			rb2d.runtimeBodyIndex = AddActiveBody(body);

		}

		return true;
	}

	bool PhysicsSystem::DestroyPhysics(EntityID entityID, NRegistry* registry)
	{
		if (registry->HasComponent<CircleCollider2DComponent>(entityID) || registry->HasComponent<BoxCollider2DComponent>(entityID))
			return true;

		Rigidbody2DComponent rb2d = registry->GetComponent<Rigidbody2DComponent>(entityID);

		RemoveActiveBody(rb2d.runtimeBodyIndex);


	}
}