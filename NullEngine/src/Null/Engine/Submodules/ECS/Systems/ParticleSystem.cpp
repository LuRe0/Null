
//------------------------------------------------------------------------------
//
// File Name:	ParticleSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ParticleSystem.h"
//#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "Null/Tools/EasingCurve.h"
#include "Null/Tools/ImGuiH.h"
#include <misc/cpp/imgui_stdlib.h>
#include "Null/Engine/Submodules/Events/IEvents.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/ParticleBatchRenderer.h"
#include <magic_enum/magic_enum.hpp>

#include <glad/glad.h> 

#include "../Entities/Entity.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


constexpr GLuint COLOR_EASE_TEXTURE_UNIT = 5;
constexpr GLuint SIZE_EASE_TEXTURE_UNIT = 6;
constexpr GLuint ROTATION_EASE_TEXTURE_UNIT = 7;
constexpr GLuint FADE_EASE_TEXTURE_UNIT = 8;
constexpr GLuint SHAPE_TEXTURE_UNIT = 9;

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	void SetShapeUniforms(ComputeShader& shader, const ParticleEmitter& emitter)
	{
		switch (emitter.spawnShapeData.shapeType)
		{
		case SpawnShape::POINT:
			shader.setVec3("u_Point", emitter.spawnShapeData.data.point.point);
			break;

		case SpawnShape::CIRCLE:
			shader.setFloat("u_CircleRadius", emitter.spawnShapeData.data.circle.radius);
			break;

		case SpawnShape::DONUT:
			shader.setFloat("u_DonutRadius1", emitter.spawnShapeData.data.donut.innerRadius);
			shader.setFloat("u_DonutRadius2", emitter.spawnShapeData.data.donut.outerRadius);
			break;

		case SpawnShape::LINE:
			shader.setVec3("u_LinePoint1", emitter.spawnShapeData.data.line.p1);
			shader.setVec3("u_LinePoint2", emitter.spawnShapeData.data.line.p2);
			break;

		case SpawnShape::RECT:
			shader.setVec2("u_RectCenter", emitter.spawnShapeData.data.rect.center);
			shader.setVec2("u_RectExtent", emitter.spawnShapeData.data.rect.extent);
			break;
		case SpawnShape::TEXTURE:
		{
			SpriteSource* spriteSource =  NSpriteSourceManager::Instance()->Get(emitter.spawnShapeData.data.texture.spriteID);
			if (spriteSource)
			{
				shader.setTexture("u_SpawnMask", spriteSource->GetTexture()->GetID(), SHAPE_TEXTURE_UNIT);
				shader.setVec2("u_MaskWorldSize", emitter.spawnShapeData.data.texture.size);
				shader.setFloat("u_MaskAlphaThreshold", emitter.spawnShapeData.data.texture.alphaThreshold);
				shader.setFloat("u_InvertMask", emitter.spawnShapeData.data.texture.invertMask);
			}
		}
		}

		shader.setBool("u_FollowParent", emitter.followParent);
	}

	bool Vec3Equal(const glm::vec3& a, const glm::vec3& b)
	{
		constexpr float EPSILON = 0.0001f;
		return glm::all(glm::lessThan(glm::abs(a - b), glm::vec3(EPSILON)));
	}

	inline bool FloatEqual(float a, float b, float epsilon = 0.0001f)
	{
		return std::abs(a - b) < epsilon;
	}


	ParticleSystem::ParticleSystem()
	{
		Require<TransformComponent>();
		Require<ParticleSystemComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<ParticleSystemComponent>(CreateParticleSystemComponent,
			[this](Entity& id) { this->ViewParticleSystemComponent(id); }, 
			WriteParticleSystemComponent, 
			AddParticleSystemComponent, DiffParticleSystemComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
		);

		m_TotalMaxParticles = 0;
		m_EmitComputeShader = 0;
		m_UpdateComputeShader = 0;
		m_InitComputeShader = 0;
	}

	void ParticleSystem::Load()
	{
		ISystem::Load();

		NComputeShaderManager* shaderMan = NComputeShaderManager::Instance();
		auto updateShader = shaderMan->Get("particleUpdate");
		m_UpdateComputeShader = dynamic_cast<ComputeShader*>(updateShader);

		auto emitShader = shaderMan->Get("particleEmit");
		m_EmitComputeShader = dynamic_cast<ComputeShader*>(emitShader);


		auto initShader = shaderMan->Get("particleInit");
		m_InitComputeShader = dynamic_cast<ComputeShader*>(initShader);
	}

	void ParticleSystem::Init()
	{
		ISystem::Init();
		NEventManager* eventManager =   NEventManager::Instance();
		NRenderer* renderer = NRenderer::Instance();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &ParticleSystem::OnEntityCreated, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ParticleSystem::OnSceneSwitched, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &ParticleSystem::OnEntityComponentAdded, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &ParticleSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityDestroyedEvent, &ParticleSystem::OnEntityDestroyed, eventManager, EventPriority::High);

		NRegistry* registry = NRegistry::Instance();


		ParticleBatchRenderer<Mesh>* batcher = renderer->AddBatcher<ParticleBatchRenderer<Mesh>>(STRID("Particle"));


		m_Batcher = dynamic_cast<ParticleBatchRenderer<Mesh>*>(batcher);

		InitParticleBuffer(GetSystemEntities(), registry);
	}

	void ParticleSystem::SetEmitterUniforms(ComputeShader& shader, const ParticleEmitter& emitter, const TransformComponent& transform)
	{
		// time
		shader.setFloat("u_DeltaTime", Time::DeltaTime());  // or pass dt if preferred
		shader.setFloat("u_Time", Time::LastTime());

		shader.setUInt("u_EmitterFlags", emitter.flags);

		// position
		shader.setVec3("u_EmitterOffset", emitter.offset);
		shader.setVec3("u_ParentPosition", transform.translation);

		// physics
		shader.setFloat("u_EmitterInitialAcceleration", emitter.initialAcceleration);
		shader.setFloat("u_EmitterInitialVelocity", emitter.initialVelocity);
		shader.setFloat("u_EmitterAngularVelocity", emitter.initialAngularVelocity);
		shader.setFloat("u_Drag", emitter.drag);

		// size
		shader.setVec2("u_EmitterStartSize", emitter.startSize);
		shader.setVec2("u_EmitterEndSize", emitter.endSize);

		// rotation
		shader.setFloat("u_EmitterStartRotation", emitter.startRotation);
		shader.setFloat("u_EmitterEndRotation", emitter.endRotation);

		// fade
		shader.setFloat("u_AlphaStart", emitter.startFade);
		shader.setFloat("u_AlphaEnd", emitter.endFade);

		// color
		shader.setVec4("u_EmitterStartColor", emitter.startColor);
		shader.setVec4("u_EmitterEndColor", emitter.endColor);

		//animation
		shader.setFloat("u_AnimDuration", emitter.animDuration);
		shader.setInt("u_AnimFrameCount", emitter.animFrameCount);
		shader.setFloat("u_StartOffset", emitter.startOffset);

		shader.setBool("u_Looping", emitter.loop);
		shader.setBool("u_PlayOnce", emitter.playOnce);
		shader.setBool("u_Reverse", emitter.reverse);
		shader.setBool("u_PingPong", emitter.pingPong);

		// lifetime
		shader.setFloat("u_EmitterMinLifetime", emitter.minLifetime);
		shader.setFloat("u_EmitterMaxLifetime", emitter.maxLifetime);
		shader.setInt("u_RandomizeLifetime", emitter.randomizeLifetime);

		// buffer offset
		shader.setInt("u_StartIndex", static_cast<int>(emitter.startIndex));


		// shape
		shader.setInt("u_EmitterShape", static_cast<int>(emitter.spawnShapeData.shapeType));

		// forces
		shader.setVec3("u_VortexCenter", emitter.vortexCenter);
		shader.setFloat("u_VortexStrength", emitter.vortexStrength);

		shader.setVec3("u_WindDirection", (glm::length(emitter.windDirection) > 0.001f) ? glm::normalize(emitter.windDirection) : emitter.windDirection);
		shader.setFloat("u_WindStrength", emitter.windStrength);

		shader.setVec3("u_AttractorPosition", emitter.attractorPosition);
		shader.setFloat("u_AttractionStrength", emitter.attractionStrength);

		shader.setVec3("u_SpinCenter", emitter.spinCenter);
		shader.setFloat("u_SpinSpeed", emitter.spinSpeed);

		shader.setVec3("u_Gravity", emitter.gravity);
		shader.setFloat("u_GravityScale", emitter.gravityScale);

		// any other uniforms...
		SetShapeUniforms(shader, emitter);
	}

	void ParticleSystem::Update(float dt)
	{
		NRegistry* registry = NRegistry::Instance();

		const GLuint workGroupSize = 256;

		for (const auto entityId : GetSystemEntities())
		{
			ParticleSystemComponent& particleSystem = registry->GetComponent<ParticleSystemComponent>(entityId);
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			if (!particleSystem.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			for (size_t i = 0; i < particleSystem.emitterCount; ++i)
			{
				ParticleEmitter& emitter = particleSystem.emitters[i];

				if (!emitter.enabled || emitter.finished)
					continue;

				// --- Pass 1: Update ---
				m_UpdateComputeShader->Bind();
				m_UpdateComputeShader->setFloat("u_DeltaTime", dt);
				m_UpdateComputeShader->setInt("u_MaxParticles", emitter.maxParticles);
				EasingCurveLib::BindToShader(emitter.colorEaseCurve, m_UpdateComputeShader->GetID(), "u_ColorEaseCurve", COLOR_EASE_TEXTURE_UNIT);
				EasingCurveLib::BindToShader(emitter.fadeEaseCurve, m_UpdateComputeShader->GetID(), "u_AlphaEaseCurve", FADE_EASE_TEXTURE_UNIT);
				EasingCurveLib::BindToShader(emitter.sizeEaseCurve, m_UpdateComputeShader->GetID(), "u_SizeEaseCurve", SIZE_EASE_TEXTURE_UNIT);
				EasingCurveLib::BindToShader(emitter.rotationEaseCurve, m_UpdateComputeShader->GetID(), "u_RotationEaseCurve", ROTATION_EASE_TEXTURE_UNIT);
				SetEmitterUniforms(*m_UpdateComputeShader, emitter, transform);

				m_ParticleSSBO.Bind(0);

				GLuint groups = (emitter.maxParticles + workGroupSize - 1) / workGroupSize;
				m_UpdateComputeShader->Dispatch(groups, 1, 1);

				// --- Pass 2: Emit ---

				if (emitter.useBurst && !emitter.bursting && emitter.burstCount > 0)
				{
					emitter.bursting = true;
					emitter.burstsRemaining = emitter.burstCount;
					emitter.burstTimer = 0.0f; // fire immediately
				}

				int emitCount = 0;

				if (emitter.useBurst)
				{
					if (emitter.bursting)
					{
						emitter.burstTimer -= dt;

						if (emitter.burstTimer <= 0.0f && emitter.burstsRemaining > 0)
						{
							// Use accumulator to determine how many particles to emit
							emitter.emitAccumulator += dt * emitter.emitRate;
							emitCount = (int)floor(emitter.emitAccumulator);
							emitter.emitAccumulator -= emitCount;

							emitter.burstsRemaining--;
							emitter.burstTimer = emitter.burstCooldown;

							if (emitter.burstsRemaining == 0)
								emitter.bursting = false;
						}
					}
				}
				else
				{
					// Normal continuous emission
					emitter.emitAccumulator += dt * emitter.emitRate;
					emitCount = (int)floor(emitter.emitAccumulator);
					emitter.emitAccumulator -= emitCount;
				}

				emitCount = std::min(emitCount, (int)emitter.maxParticles);
				emitter.emitCount = emitCount;

				if (emitCount > 0)
				{
					m_EmitComputeShader->Bind();
					m_EmitComputeShader->setInt("u_EmitCount", emitCount);
					m_EmitComputeShader->setInt("u_MaxParticles", emitter.maxParticles);
					m_EmitComputeShader->setFloat("u_DeltaTime", dt);

					m_EmitComputeShader->setFloat("u_EmitterInitialLifetime", emitter.initialLifetime);
					m_EmitComputeShader->setVec2("u_EmitterInitialSize", emitter.initialSize);
					m_EmitComputeShader->setVec4("u_EmitterInitialColor", emitter.initialColor);
					m_EmitComputeShader->setFloat("u_EmitterInitialRotation", emitter.initialRotation);
					m_EmitComputeShader->setInt("u_EmitterInitialFrame", emitter.initialFrame);

					SpriteSource* spriteSource = NSpriteSourceManager::Instance()->Get(emitter.spriteSourceID);

					if (spriteSource)
					{
						auto* tex = spriteSource->GetTexture();
						uint32_t compactID = NEngine::Instance()
							.Get<NTextureManager>()->GetTextureIndex(spriteSource->GetName());

						m_EmitComputeShader->setInt("u_EmitterTextureIndex",compactID);


						m_EmitComputeShader->setVec2("u_Dimensions", spriteSource->GetSize());

					}
					else
					{
						m_EmitComputeShader->setInt("u_EmitterTextureIndex", -1);

					}
				

					//m_EmitComputeShader->setInt("u_EmitterInitialFrame", emitter.initialFrame);

					SetEmitterUniforms(*m_EmitComputeShader, emitter, transform);

					m_ParticleSSBO.Bind(0);

					GLuint emitGroups = (emitCount + workGroupSize - 1) / workGroupSize;
					m_EmitComputeShader->Dispatch(emitGroups, 1, 1);
				}
			}
		}
	}

	void ParticleSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void ParticleSystem::RenderImGui()
	{
		ISystem::RenderImGui();
		ImGui::Text("Max Particles:", m_TotalMaxParticles);
	}

	void ParticleSystem::Render()
	{
		NRenderer* renderer = NRenderer::Instance();

		NRegistry* registry = NRegistry::Instance();

		NCameraManager* camManager = NCameraManager::Instance();

		glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();


		for (const auto entityId : GetSystemEntities())
		{
			ParticleSystemComponent& particleSystem = registry->GetComponent<ParticleSystemComponent>(entityId);
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			if (!particleSystem.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			for (size_t i = 0; i < particleSystem.emitterCount; ++i)
			{
				ParticleEmitter& emitter = particleSystem.emitters[i];
				if (!emitter.enabled || emitter.finished)
					continue;


				glm::vec4 worldPosition = transform.transformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

				// Transform the world position to camera space
				glm::vec4 cameraSpacePosition = viewMatrix * worldPosition;

				// The depth is the z-component of the camera space position
				float depth = cameraSpacePosition.z;

				SpriteSource* spriteSource = NSpriteSourceManager::Instance()->Get(emitter.spriteSourceID);
				//model, mesh, spritesrc, tint, shadername, frameindex, entity
				renderer->AddRenderCall(RenderCommandTypes::Particles, std::make_unique<ParticleData>(spriteSource, depth));
			}
		}
	}

	void ParticleSystem::Unload()
	{

	}

	void ParticleSystem::Shutdown()
	{
	}

	void ParticleSystem::RegisterToScripAPI(sol::state& lua)
	{

	}


	void ParticleSystem::CreateParticleSystemComponent(void* component, const nlohmann::json& json)
	{
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();

		auto* comp = static_cast<ParticleSystemComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->nameID = STRID(jsonWrapper.GetString("name", ""));

			if (json.contains("Emitters") && json["Emitters"].is_array())
			{
				for (const auto& emitterJson : json["Emitters"])
				{

					JsonReader emitterJsonWrapper(emitterJson);

					ParticleEmitter emitter;

					emitter.nameID = STRID(emitterJsonWrapper.GetString("name", ""));
					emitter.emitterID = emitterJsonWrapper.GetInt("emitterID", 0);

					emitter.offset = emitterJsonWrapper.GetVec3("offset", glm::vec3(0.0f));
					emitter.emitRate = emitterJsonWrapper.GetFloat("emissionRate", 10.0f);

					emitter.minLifetime = emitterJsonWrapper.GetFloat("minLifetime", 5.0f);
					emitter.maxLifetime = emitterJsonWrapper.GetFloat("maxLifetime", 5.0f);
					emitter.randomizeLifetime = emitterJsonWrapper.GetBool("randomizeLifetime", false);

					emitter.drag = emitterJsonWrapper.GetFloat("drag", 0.01f);

					emitter.maxParticles = emitterJsonWrapper.GetInt("maxParticles", 1000);

					emitter.flags = emitterJsonWrapper.GetInt("flags", PARTICLE_FLAG_NONE);
					emitter.enabled = emitterJsonWrapper.GetBool("enabled", true);

					emitter.initialVelocity = emitterJsonWrapper.GetFloat("initialVelocity", 0.0f);
					emitter.initialAngularVelocity = emitterJsonWrapper.GetFloat("initialAngularVelocity", 0.0f);
					emitter.initialAcceleration = emitterJsonWrapper.GetFloat("initialAcceleration", 0.0f);
					emitter.initialSize = emitterJsonWrapper.GetVec2("initialSize", emitter.initialSize);
					emitter.initialColor = emitterJsonWrapper.GetVec4("initialColor", emitter.initialColor);
					emitter.initialFrame = emitterJsonWrapper.GetInt("initialFrame", emitter.initialFrame);
					emitter.initialLifetime = emitterJsonWrapper.GetFloat("initialLifetime", emitter.initialLifetime);
					emitter.initialRotation = emitterJsonWrapper.GetFloat("initialRotation", emitter.initialRotation);

					glm::vec2 dimension = emitterJsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
					auto src = emitterJsonWrapper.GetString("texture", "");
					if (!src.empty())
					{
						emitter.spriteSourceID = STRID(src);
						spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
					}
					else
						emitter.spriteSourceID = 0;

					//size
					emitter.startSize = emitterJsonWrapper.GetVec2("startSize", glm::vec2(1.0f, 0.0f));
					emitter.endSize = emitterJsonWrapper.GetVec2("endSize", glm::vec2(1.0f, 0.0f));

					auto sizeCurve = emitterJsonWrapper.GetJSONObject("sizeCurve");
					emitter.sizeEaseCurve.type = static_cast<NULLENGINE::EasingType>(sizeCurve.GetInt("type", 0));

					std::vector<float> svals = sizeCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.sizeEaseCurve.values), std::end(emitter.sizeEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < svals.size(); ++i)
						emitter.sizeEaseCurve.values[i] = svals[i];

					//color
					emitter.startColor = emitterJsonWrapper.GetVec4("startColor", glm::vec4(1.0f));
					emitter.endColor = emitterJsonWrapper.GetVec4("endColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

					auto colorCurve = emitterJsonWrapper.GetJSONObject("colorCurve");
					emitter.colorEaseCurve.type = static_cast<NULLENGINE::EasingType>(colorCurve.GetInt("type", 0));

					std::vector<float> cvals = colorCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.colorEaseCurve.values), std::end(emitter.colorEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < cvals.size(); ++i)
						emitter.colorEaseCurve.values[i] = cvals[i];


					//rotation
					emitter.startRotation = emitterJsonWrapper.GetFloat("startRotation", 0.0f);
					emitter.endRotation = emitterJsonWrapper.GetFloat("endRotation", 0.0f);
					auto rotationCurve = emitterJsonWrapper.GetJSONObject("rotationCurve");
					emitter.rotationEaseCurve.type = static_cast<NULLENGINE::EasingType>(colorCurve.GetInt("type", 0));

					std::vector<float> rvals = rotationCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.rotationEaseCurve.values), std::end(emitter.rotationEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < rvals.size(); ++i)
						emitter.rotationEaseCurve.values[i] = rvals[i];


					//fade
					emitter.startFade = emitterJsonWrapper.GetFloat("startFade", 0.0f);
					emitter.endFade = emitterJsonWrapper.GetFloat("endFade", 0.0f);

					//animation
					emitter.animDuration = emitterJsonWrapper.GetFloat("animDuration", 1.0f);
					emitter.animFrameCount = emitterJsonWrapper.GetInt("animFrameCount", 1.0f);
					emitter.loop = emitterJsonWrapper.GetBool("loop", true);
					emitter.playOnce = emitterJsonWrapper.GetBool("playOnce", false);
					emitter.reverse = emitterJsonWrapper.GetBool("reverse", false);
					emitter.pingPong = emitterJsonWrapper.GetBool("pingPong", false);
					emitter.startOffset = emitterJsonWrapper.GetFloat("startOffset", 0.0f);


					auto fadeCurve = emitterJsonWrapper.GetJSONObject("fadeCurve");
					emitter.fadeEaseCurve.type = static_cast<NULLENGINE::EasingType>(colorCurve.GetInt("type", 0));

					std::vector<float> fvals = fadeCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.fadeEaseCurve.values), std::end(emitter.fadeEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < fvals.size(); ++i)
						emitter.fadeEaseCurve.values[i] = fvals[i];


					emitter.vortexCenter = emitterJsonWrapper.GetVec3("vortexCenter", glm::vec3(0.0f));
					emitter.vortexStrength = emitterJsonWrapper.GetFloat("vortexStrength", 0.0f);

					emitter.windDirection = emitterJsonWrapper.GetVec3("windDirection", glm::vec3(0.0f));
					emitter.windStrength = emitterJsonWrapper.GetFloat("windStrength", 0.0f);

					emitter.attractorPosition = emitterJsonWrapper.GetVec3("attractorPosition", glm::vec3(0.0f));
					emitter.attractionStrength = emitterJsonWrapper.GetFloat("attractionStrength", 0.0f);

					emitter.gravity = emitterJsonWrapper.GetVec3("gravity", glm::vec3(0.0f));
					emitter.gravityScale = emitterJsonWrapper.GetFloat("gravityScale", 0.0f);

					emitter.spinCenter = emitterJsonWrapper.GetVec3("spinCenter", glm::vec3(0.0f));
					emitter.spinSpeed = emitterJsonWrapper.GetFloat("spinSpeed", 0.0f);

					//burst
					emitter.useBurst = emitterJsonWrapper.GetBool("useBurst", false);
					emitter.burstCount = emitterJsonWrapper.GetInt("burstCount", 0);
					emitter.burstCooldown = emitterJsonWrapper.GetFloat("burstCooldown", 0.0f);

					emitter.spawnShapeData.shapeType = static_cast<SpawnShape>(emitterJsonWrapper.GetInt("shape", static_cast<int>(SpawnShape::POINT)));

					switch (emitter.spawnShapeData.shapeType)
					{
					case SpawnShape::LINE:
						if (emitterJsonWrapper.HasData("line"))
						{
							auto lineObj = emitterJsonWrapper.GetJSONObject("line");
							emitter.spawnShapeData.data.line.p1 = lineObj.GetVec3("p1", glm::vec3(0.0f));
							emitter.spawnShapeData.data.line.p2 = lineObj.GetVec3("p2", glm::vec3(0.0f));
						}
						break;

					case SpawnShape::CIRCLE:
						if (emitterJsonWrapper.HasData("circle"))
						{
							auto circleObj = emitterJsonWrapper.GetJSONObject("circle");
							emitter.spawnShapeData.data.circle.radius = circleObj.GetFloat("radius", 1.0f);
						}
						break;

					case SpawnShape::DONUT:
						if (emitterJsonWrapper.HasData("donut"))
						{
							auto donutObj = emitterJsonWrapper.GetJSONObject("donut");
							emitter.spawnShapeData.data.donut.innerRadius = donutObj.GetFloat("innerRadius", 0.5f);
							emitter.spawnShapeData.data.donut.outerRadius = donutObj.GetFloat("outerRadius", 1.0f);
						}
						break;

					case SpawnShape::POINT:
						if (emitterJsonWrapper.HasData("point"))
						{
							auto pointObj = emitterJsonWrapper.GetJSONObject("point");
							emitter.spawnShapeData.data.point.point = pointObj.GetVec3("point", glm::vec3(0.0f));
						}
						break;

					case SpawnShape::RECT:
						if (emitterJsonWrapper.HasData("rect"))
						{
							auto rectObj = emitterJsonWrapper.GetJSONObject("rect");

							emitter.spawnShapeData.data.rect.center = rectObj.GetVec2("rectCenter", emitter.spawnShapeData.data.rect.center);
							emitter.spawnShapeData.data.rect.extent = rectObj.GetVec2("rectExtent", emitter.spawnShapeData.data.rect.extent);
						}
						break;
					case SpawnShape::TEXTURE:
						if (emitterJsonWrapper.HasData("textureShape"))
						{
							auto texureObj = emitterJsonWrapper.GetJSONObject("textureShape");
							emitter.spawnShapeData.data.texture.size = texureObj.GetVec2("worldSize", emitter.spawnShapeData.data.texture.size);
							emitter.spawnShapeData.data.texture.alphaThreshold = texureObj.GetFloat("alphaThreshold", 0.1f);
							emitter.spawnShapeData.data.texture.invertMask = texureObj.GetFloat("invertMask", 0.0f);
							auto src = texureObj.GetString("shapeTexture", "");
							if (!src.empty())
							{
								emitter.spawnShapeData.data.texture.spriteID = STRID(src);

								spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
							}
							else
								emitter.spawnShapeData.data.texture.spriteID = 0;
						}
						break;
					default:
						break;
					}

					comp->emitters[comp->emitterCount++] = emitter;
				}
			}
		}


		// Add or update component in registry
		//NComponentFactory* componentFactory = NComponentFactory::Instance();
	}



	void ParticleSystem::AddParticleSystemComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ParticleSystemComponent*>(component);
		componentFactory->AddOrUpdate<ParticleSystemComponent>(id, comp, registry, comp->emitters, comp->nameID, comp->emitterCount);
	}

	JSON DiffEmitter(const ParticleEmitter& a, const ParticleEmitter& b)
	{
		JSON diff;

		//if (a.name != b.name)
		//	diff["name"] = b.name;

		//if (a.emitterID != b.emitterID)
		//	diff["emitterID"] = b.emitterID;

		//// Texture comparison (handle nullptr)
		//std::string texA = a.spriteSource ? a.spriteSource->GetName() : "";
		//std::string texB = b.spriteSource ? b.spriteSource->GetName() : "";
		//if (texA != texB)
		//	diff["texture"] = texB;

		//// Dimension: compare vectors or arrays of int
		//if (a.spriteSource == nullptr && b.spriteSource == nullptr)
		//{
		//	// both null => dimension default {1,1}, no diff needed
		//}
		//else if (a.spriteSource == nullptr || b.spriteSource == nullptr)
		//{
		//	diff["dimension"] = nlohmann::json::array({ b.spriteSource->GetRows(), b.spriteSource->GetCols() });
		//}
		//else
		//{
		//	if (a.spriteSource->GetRows() != b.spriteSource->GetRows() ||
		//		a.spriteSource->GetCols() != b.spriteSource->GetCols())
		//	{
		//		diff["dimension"] = nlohmann::json::array({ b.spriteSource->GetRows(), b.spriteSource->GetCols() });
		//	}
		//}

		//// Simple vector3 comparisons:
		//if (!Vec3Equal(a.offset, b.offset))
		//	diff["offset"] = { b.offset.x, b.offset.y, b.offset.z };

		//if (a.emitRate != b.emitRate)
		//	diff["emissionRate"] = b.emitRate;

		//if (a.minLifetime != b.minLifetime)
		//	diff["minLifetime"] = b.minLifetime;

		//if (a.maxLifetime != b.maxLifetime)
		//	diff["maxLifetime"] = b.maxLifetime;

		//if (a.randomizeLifetime != b.randomizeLifetime)
		//	diff["randomizeLifetime"] = b.randomizeLifetime;

		//if (a.drag != b.drag)
		//	diff["drag"] = b.drag;

		//if (a.maxParticles != b.maxParticles)
		//	diff["maxParticles"] = static_cast<int>(b.maxParticles);

		//if (a.bufferOffset != b.bufferOffset)
		//	diff["bufferOffset"] = static_cast<int>(b.bufferOffset);

		//if (a.flags != b.flags)
		//	diff["flags"] = b.flags;

		//if (a.enabled != b.enabled)
		//	diff["enabled"] = b.enabled;

		//if (a.finished != b.finished)
		//	diff["finished"] = b.finished;

		//// Now many more simple scalar and vector fields:

		//// initialVelocity, initialAngularVelocity, initialAcceleration, initialLifetime, initialRotation

		//if (a.initialVelocity != b.initialVelocity)
		//	diff["initialVelocity"] = b.initialVelocity;

		//if (a.initialAngularVelocity != b.initialAngularVelocity)
		//	diff["initialAngularVelocity"] = b.initialAngularVelocity;

		//if (a.initialAcceleration != b.initialAcceleration)
		//	diff["initialAcceleration"] = b.initialAcceleration;

		//if (a.initialLifetime != b.initialLifetime)
		//	diff["initialLifetime"] = b.initialLifetime;

		//if (a.initialRotation != b.initialRotation)
		//	diff["initialRotation"] = b.initialRotation;

		//// initialSize vector2
		//if (a.initialSize.x != b.initialSize.x || a.initialSize.y != b.initialSize.y)
		//	diff["initialSize"] = { b.initialSize.x, b.initialSize.y };

		//if (a.initialFrame != b.initialFrame)
		//	diff["initialFrame"] = b.initialFrame;

		//// initialColor (RGBA floats)
		//if (a.initialColor != b.initialColor)
		//	diff["initialColor"] = { b.initialColor.r, b.initialColor.g, b.initialColor.b, b.initialColor.a };

		//// startSize & endSize (vec2)
		//if (a.startSize != b.startSize)
		//	diff["startSize"] = { b.startSize.x, b.startSize.y };
		//if (a.endSize != b.endSize)
		//	diff["endSize"] = { b.endSize.x, b.endSize.y };

		//// Curves: compare type and values arrays
		//if (a.sizeEaseCurve.GetType() != b.sizeEaseCurve.GetType())
		//	diff["sizeCurve"]["type"] = b.sizeEaseCurve.GetType();

		//if (std::vector<float>(std::begin(a.sizeEaseCurve.values), std::end(a.sizeEaseCurve.values)) !=
		//	std::vector<float>(std::begin(b.sizeEaseCurve.values), std::end(b.sizeEaseCurve.values)))
		//{
		//	diff["sizeCurve"]["values"] = std::vector<float>(std::begin(b.sizeEaseCurve.values), std::end(b.sizeEaseCurve.values));
		//}

		//// Similarly for colorCurve
		//if (a.colorEaseCurve.GetType() != b.colorEaseCurve.GetType())
		//	diff["colorCurve"]["type"] = b.colorEaseCurve.GetType();

		//if (std::vector<float>(std::begin(a.colorEaseCurve.values), std::end(a.colorEaseCurve.values)) !=
		//	std::vector<float>(std::begin(b.colorEaseCurve.values), std::end(b.colorEaseCurve.values)))
		//{
		//	diff["colorCurve"]["values"] = std::vector<float>(std::begin(b.colorEaseCurve.values), std::end(b.colorEaseCurve.values));
		//}

		//// startColor & endColor
		//if (a.startColor != b.startColor)
		//	diff["startColor"] = { b.startColor.r, b.startColor.g, b.startColor.b, b.startColor.a };

		//if (a.endColor != b.endColor)
		//	diff["endColor"] = { b.endColor.r, b.endColor.g, b.endColor.b, b.endColor.a };

		//// rotationCurve same pattern as above
		//if (a.rotationEaseCurve.GetType() != b.rotationEaseCurve.GetType())
		//	diff["rotationCurve"]["type"] = b.rotationEaseCurve.GetType();

		//if (std::vector<float>(std::begin(a.rotationEaseCurve.values), std::end(a.rotationEaseCurve.values)) !=
		//	std::vector<float>(std::begin(b.rotationEaseCurve.values), std::end(b.rotationEaseCurve.values)))
		//{
		//	diff["rotationCurve"]["values"] = std::vector<float>(std::begin(b.rotationEaseCurve.values), std::end(b.rotationEaseCurve.values));
		//}

		//// startRotation, endRotation
		//if (a.startRotation != b.startRotation)
		//	diff["startRotation"] = b.startRotation;

		//if (a.endRotation != b.endRotation)
		//	diff["endRotation"] = b.endRotation;

		//// fadeCurve
		//if (a.fadeEaseCurve.GetType() != b.fadeEaseCurve.GetType())
		//	diff["fadeCurve"]["type"] = b.fadeEaseCurve.GetType();

		//if (std::vector<float>(std::begin(a.fadeEaseCurve.values), std::end(a.fadeEaseCurve.values)) !=
		//	std::vector<float>(std::begin(b.fadeEaseCurve.values), std::end(b.fadeEaseCurve.values)))
		//{
		//	diff["fadeCurve"]["values"] = std::vector<float>(std::begin(b.fadeEaseCurve.values), std::end(b.fadeEaseCurve.values));
		//}

		//// startFade, endFade
		//if (a.startFade != b.startFade)
		//	diff["startFade"] = b.startFade;

		//if (a.endFade != b.endFade)
		//	diff["endFade"] = b.endFade;

		//// vortexCenter (vec3), vortexStrength (float)
		//if (!Vec3Equal(a.vortexCenter, b.vortexCenter))
		//	diff["vortexCenter"] = { b.vortexCenter.x, b.vortexCenter.y, b.vortexCenter.z };

		//if (a.vortexStrength != b.vortexStrength)
		//	diff["vortexStrength"] = b.vortexStrength;

		//// windDirection, windStrength
		//if (!Vec3Equal(a.windDirection, b.windDirection))
		//	diff["windDirection"] = { b.windDirection.x, b.windDirection.y, b.windDirection.z };

		//if (a.windStrength != b.windStrength)
		//	diff["windStrength"] = b.windStrength;

		//// AttractorPosition, AttractionStrength
		//if (!Vec3Equal(a.AttractorPosition, b.AttractorPosition))
		//	diff["attractorPosition"] = { b.AttractorPosition.x, b.AttractorPosition.y, b.AttractorPosition.z };

		//if (a.AttractionStrength != b.AttractionStrength)
		//	diff["attractionStrength"] = b.AttractionStrength;

		//// Animation related fields
		//if (a.animDuration != b.animDuration)
		//	diff["animDuration"] = b.animDuration;

		//if (a.animFrameCount != b.animFrameCount)
		//	diff["animFrameCount"] = b.animFrameCount;

		//if (a.loop != b.loop)
		//	diff["loop"] = b.loop;

		//if (a.playOnce != b.playOnce)
		//	diff["playOnce"] = b.playOnce;

		//if (a.reverse != b.reverse)
		//	diff["reverse"] = b.reverse;

		//if (a.pingPong != b.pingPong)
		//	diff["pingPong"] = b.pingPong;

		//if (a.startOffset != b.startOffset)
		//	diff["startOffset"] = b.startOffset;

		//// Gravity vector3 and gravityScale
		//if (!Vec3Equal(a.gravity, b.gravity))
		//	diff["gravity"] = { b.gravity.x, b.gravity.y, b.gravity.z };

		//if (a.gravityScale != b.gravityScale)
		//	diff["gravityScale"] = b.gravityScale;

		//// Spin center & speed
		//if (!Vec3Equal(a.spinCenter, b.spinCenter))
		//	diff["spinCenter"] = { b.spinCenter.x, b.spinCenter.y, b.spinCenter.z };

		//if (a.spinSpeed != b.spinSpeed)
		//	diff["spinSpeed"] = b.spinSpeed;

		//// Burst stuff
		//if (a.useBurst != b.useBurst)
		//	diff["useBurst"] = b.useBurst;

		//if (a.burstCount != b.burstCount)
		//	diff["burstCount"] = b.burstCount;

		//if (a.burstCooldown != b.burstCooldown)
		//	diff["burstCooldown"] = b.burstCooldown;

		//// Shape enum and shape-specific fields:
		//if (a.shape != b.shape)
		//{
		//	diff["shape"] = static_cast<int>(b.shape);
		//	// Include full new shape details here if you want.
		//}
		//else
		//{
		//	// Same shape - compare shape-specific data
		//	switch (b.shape)
		//	{
		//	case SpawnShape::LINE:
		//		if (!Vec3Equal(a.line.point1, b.line.point1))
		//		{
		//			diff["line"]["point1_x"] = b.line.point1.x;
		//			diff["line"]["point1_y"] = b.line.point1.y;
		//			diff["line"]["point1_z"] = b.line.point1.z;
		//		}
		//		if (!Vec3Equal(a.line.point2, b.line.point2))
		//		{
		//			diff["line"]["point2_x"] = b.line.point2.x;
		//			diff["line"]["point2_y"] = b.line.point2.y;
		//			diff["line"]["point2_z"] = b.line.point2.z;
		//		}
		//		break;

		//	case SpawnShape::CIRCLE:
		//		if (a.circle.radius != b.circle.radius)
		//			diff["circle"]["radius"] = b.circle.radius;
		//		break;

		//	case SpawnShape::DONUT:
		//		if (a.donut.radius1 != b.donut.radius1)
		//			diff["donut"]["radius1"] = b.donut.radius1;
		//		if (a.donut.radius2 != b.donut.radius2)
		//			diff["donut"]["radius2"] = b.donut.radius2;
		//		break;

		//	case SpawnShape::POINT:
		//		if (!Vec3Equal(a.point.point, b.point.point))
		//		{
		//			diff["point"]["x"] = b.point.point.x;
		//			diff["point"]["y"] = b.point.point.y;
		//			diff["point"]["z"] = b.point.point.z;
		//		}
		//		break;

		//	case SpawnShape::RECT:
		//		if (a.rect.center != b.rect.center)
		//			diff["rect"]["rectCenter"] = { b.rect.center.x, b.rect.center.y };
		//		if (a.rect.extent != b.rect.extent)
		//			diff["rect"]["rectExtent"] = { b.rect.extent.x, b.rect.extent.y };
		//		break;

		//	case SpawnShape::TEXTURE:
		//		if (a.texture.worldSize != b.texture.worldSize)
		//			diff["textureShape"]["worldSize"] = { b.texture.worldSize.x, b.texture.worldSize.y };

		//		if (a.texture.alphaThreshold != b.texture.alphaThreshold)
		//			diff["textureShape"]["alphaThreshold"] = b.texture.alphaThreshold;

		//		if (a.texture.invertMask != b.texture.invertMask)
		//			diff["textureShape"]["invertMask"] = b.texture.invertMask;

		//		{
		//			std::string texShapeA = a.texture.spriteSource ? a.texture.spriteSource->GetName() : "";
		//			std::string texShapeB = b.texture.spriteSource ? b.texture.spriteSource->GetName() : "";
		//			if (texShapeA != texShapeB)
		//				diff["textureShape"]["shapeTexture"] = texShapeB;
		//		}
		//		break;

		//	default:
		//		break;
		//	}
		//}

		return diff;
	}

	JSON SerializeEmitter(const ParticleEmitter& emitter)
	{
		nlohmann::json e;

		e["name"] = STRFROM(emitter.nameID);
		e["emitterID"] = emitter.emitterID;


		SpriteSource* spriteSource = NSpriteSourceManager::Instance()->Get(emitter.spriteSourceID);
		if (spriteSource)
		{

			e["texture"] = spriteSource->GetName();
			e["dimension"] = spriteSource == nullptr
				? nlohmann::json::array({ 1, 1 })
				: nlohmann::json::array({ spriteSource->GetRows(), spriteSource->GetCols() });
		}
		e["offset"] = { emitter.offset.x, emitter.offset.y, emitter.offset.z };
		e["emissionRate"] = emitter.emitRate;
		e["minLifetime"] = emitter.minLifetime;
		e["maxLifetime"] = emitter.maxLifetime;
		e["randomizeLifetime"] = emitter.randomizeLifetime;
		e["drag"] = emitter.drag;

		e["maxParticles"] = static_cast<int>(emitter.maxParticles);

		e["bufferOffset"] = static_cast<int>(emitter.bufferOffset);

		e["flags"] = emitter.flags;
		e["enabled"] = emitter.enabled;

		e["finished"] = emitter.finished;

		e["initialVelocity"] = emitter.initialVelocity;
		e["initialAngularVelocity"] = emitter.initialAngularVelocity;
		e["initialAcceleration"] = emitter.initialAcceleration;
		e["initialLifetime"] = emitter.initialLifetime;
		e["initialSize"] = { emitter.initialSize.x, emitter.initialSize.y };
		e["initialRotation"] = emitter.initialRotation;
		e["initialFrame"] = emitter.initialFrame;
		e["initialColor"] = { emitter.initialColor.r, emitter.initialColor.g,emitter.initialColor.b, emitter.initialColor.a };

		e["startSize"] = { emitter.startSize.x, emitter.startSize.y };
		e["endSize"] = { emitter.endSize.x, emitter.endSize.y };
		e["sizeCurve"]["type"] = emitter.sizeEaseCurve.type;
		e["sizeCurve"]["values"] = std::vector<float>(std::begin(emitter.sizeEaseCurve.values), std::end(emitter.sizeEaseCurve.values));

		e["startColor"] = { emitter.startColor.r, emitter.startColor.g, emitter.startColor.b, emitter.startColor.a };
		e["endColor"] = { emitter.endColor.r, emitter.endColor.g, emitter.endColor.b, emitter.endColor.a };
		e["colorCurve"]["type"] = emitter.colorEaseCurve.type;
		e["colorCurve"]["values"] = std::vector<float>(std::begin(emitter.colorEaseCurve.values), std::end(emitter.colorEaseCurve.values));

		e["startRotation"] = emitter.startRotation;
		e["endRotation"] = emitter.endRotation;
		e["rotationCurve"]["type"] = emitter.rotationEaseCurve.type;
		e["rotationCurve"]["values"] = std::vector<float>(std::begin(emitter.rotationEaseCurve.values), std::end(emitter.rotationEaseCurve.values));


		e["startFade"] = emitter.startFade;
		e["endFade"] = emitter.endFade;
		e["fadeCurve"]["type"] = emitter.fadeEaseCurve.type;
		e["fadeCurve"]["values"] = std::vector<float>(std::begin(emitter.fadeEaseCurve.values), std::end(emitter.fadeEaseCurve.values));

		e["vortexCenter"] = { emitter.vortexCenter.x, emitter.vortexCenter.y, emitter.vortexCenter.z };
		e["vortexStrength"] = emitter.vortexStrength;

		e["windDirection"] = { emitter.windDirection.x, emitter.windDirection.y, emitter.windDirection.z };
		e["windStrength"] = emitter.windStrength;

		e["attractorPosition"] = { emitter.attractorPosition.x, emitter.attractorPosition.y, emitter.attractorPosition.z };
		e["attractionStrength"] = emitter.attractionStrength;


		e["animDuration"] = emitter.animDuration;
		e["animFrameCount"] = emitter.animFrameCount;
		e["loop"] = emitter.loop;
		e["playOnce"] = emitter.playOnce;
		e["reverse"] = emitter.reverse;
		e["pingPong"] = emitter.pingPong;
		e["startOffset"] = emitter.startOffset;



		e["gravity"] = { emitter.gravity.x, emitter.gravity.y, emitter.gravity.z };
		e["gravityScale"] = emitter.gravityScale;

		e["spinCenter"] = { emitter.spinCenter.x, emitter.spinCenter.y, emitter.spinCenter.z };
		e["spinSpeed"] = emitter.spinSpeed;

		e["useBurst"] = emitter.useBurst;
		e["burstCount"] = emitter.burstCount;
		e["burstCooldown"] = emitter.burstCooldown;

		e["shape"] = static_cast<int>(emitter.spawnShapeData.shapeType);

		// Write each shape struct depending on current shape
		switch (emitter.spawnShapeData.shapeType)
		{
		case SpawnShape::LINE:
			e["line"]["p1"] = { emitter.spawnShapeData.data.line.p1.x, emitter.spawnShapeData.data.line.p1.y, emitter.spawnShapeData.data.line.p1.z };
			e["line"]["p2"] = { emitter.spawnShapeData.data.line.p2.x, emitter.spawnShapeData.data.line.p2.y, emitter.spawnShapeData.data.line.p2.z };
			break;

		case SpawnShape::CIRCLE:
			e["circle"]["radius"] = emitter.spawnShapeData.data.circle.radius;
			break;

		case SpawnShape::DONUT:
			e["donut"]["innerRadius"] = emitter.spawnShapeData.data.donut.innerRadius;
			e["donut"]["outerRadius"] = emitter.spawnShapeData.data.donut.outerRadius;
			break;

		case SpawnShape::POINT:
			e["point"]["point"] = { emitter.spawnShapeData.data.point.point.x, emitter.spawnShapeData.data.point.point.y, emitter.spawnShapeData.data.point.point.z };
			break;

		case SpawnShape::RECT:
			e["rect"]["rectCenter"] = { emitter.spawnShapeData.data.rect.center.x, emitter.spawnShapeData.data.rect.center.y };
			e["rect"]["rectExtent"] = { emitter.spawnShapeData.data.rect.extent.x, emitter.spawnShapeData.data.rect.extent.y };
			break;
		case SpawnShape::TEXTURE:
			e["textureShape"]["worldSize"] = { emitter.spawnShapeData.data.texture.size.x, emitter.spawnShapeData.data.texture.size.y };
			e["textureShape"]["alphaThreshold"] = emitter.spawnShapeData.data.texture.alphaThreshold;
			e["textureShape"]["invertMask"] = emitter.spawnShapeData.data.texture.invertMask;
			e["textureShape"]["shapeTexture"] = STRFROM(emitter.spawnShapeData.data.texture.spriteID);
			break;
		default:
			break;
		}


		return e;
	}


	JSON ParticleSystem::WriteParticleSystemComponent(const void* component)
	{
		nlohmann::json json;

		auto& psComp = *static_cast<const ParticleSystemComponent*>(component);

		//if (!psComp.componentFlags.IsSet(ComponentFlags_Serialized))
		//	return json;

		json["ParticleSystem"]["name"] = STRFROM(psComp.nameID);

		// Emitters array
		nlohmann::json emittersJson = nlohmann::json::array();

		for (size_t i = 0; i < psComp.emitterCount; ++i)
		{
			const auto& emitter = psComp.emitters[i];
			emittersJson.push_back(SerializeEmitter(emitter));
		}
		// Add emitters to the main JSON object
		json["ParticleSystem"]["Emitters"] = emittersJson;

		return json;
	}


	JSON ParticleSystem::DiffParticleSystemComponent(const void* base, const void* modified)
	{
		auto* a = static_cast<const ParticleSystemComponent*>(base);
		auto* b = static_cast<const ParticleSystemComponent*>(modified);

		JSON diff;
		//JSON psJson;

		//// Check name
		//if (a->m_Name != b->m_Name)
		//	psJson["name"] = b->m_Name;

		//// Compare emitters count first
		//if (a->m_Emitters.size() != b->m_Emitters.size())
		//{
		//	// Output full emitters array from modified since structure changed
		//	nlohmann::json emittersJson = nlohmann::json::array();

		//	for (const auto& emitter : b->m_Emitters)
		//	{
		//		emittersJson.push_back(SerializeEmitter(emitter));
		//	}
		//	psJson["Emitters"] = emittersJson;
		//}
		//else
		//{
		//	// Same size, diff each emitter
		//	nlohmann::json emittersJson = nlohmann::json::array();

		//	bool anyEmitterDiff = false;
		//	for (size_t i = 0; i < a->m_Emitters.size(); ++i)
		//	{
		//		JSON emitterDiff = DiffEmitter(a->m_Emitters[i], b->m_Emitters[i]);
		//		if (!emitterDiff.empty())
		//		{
		//			anyEmitterDiff = true;
		//			emittersJson.push_back(emitterDiff);
		//		}
		//		else
		//		{
		//			// Could push empty or skip — decide based on your use case
		//			emittersJson.push_back(JSON::object());
		//		}
		//	}

		//	if (anyEmitterDiff)
		//		psJson["Emitters"] = emittersJson;
		//}

		//if (!psJson.empty())
		//	diff["ParticleSystem"] = psJson;

		return diff;
	}



	void ParticleSystem::DrawCoreSettings(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));
		ImGui::DragFloat("Emission Rate", &emitter.emitRate, 0.1f);
		if (ImGui::DragInt("Max Particles", reinterpret_cast<int*>(&emitter.maxParticles), 10, 1, 100000)) {
			NRegistry* registry = NRegistry::Instance();
			InitParticleBuffer(GetSystemEntities(), registry);
		}
		ImGui::PopID();
	}

	void ParticleSystem::DrawColorConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::ColorEdit4("Start Color", glm::value_ptr(emitter.startColor));
		ImGui::ColorEdit4("End Color", glm::value_ptr(emitter.endColor));
		EasingCurveLib::DrawEditorUI(emitter.colorEaseCurve, "Color Curve");

		ImGui::PopID();


	}

	void ParticleSystem::DrawLifetimeSettings(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::Checkbox("Randomize Lifetime", &emitter.randomizeLifetime);
		if (emitter.randomizeLifetime)
			ImGui::DragFloat("Min Lifetime", &emitter.minLifetime, 0.1f);
		ImGui::DragFloat("Max Lifetime", &emitter.maxLifetime, 0.1f);

		ImGui::PopID();

	}

	void ParticleSystem::DrawPhysicsConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat("Initial Velocity", &emitter.initialVelocity, 0.1f);
		ImGui::DragFloat("Initial Angular Velocity", &emitter.initialAngularVelocity, 0.1f);
		ImGui::DragFloat("Initial Acceleration", &emitter.initialAcceleration, 0.1f);
		ImGui::DragFloat("Drag", &emitter.drag, 0.01f);
		//ImGui::DragFloat("Burst Count", &emitter.burstCount, 1.0f);

		ImGui::PopID();

	}


	void ParticleSystem::DrawInitConfig(ParticleEmitter& emitter, size_t i)
	{
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		NTextureManager* texureManager = NTextureManager::Instance();

		ImGui::PushID(static_cast<int>(i));

		ImGui::Text("Initial State");

		ImGui::Separator();

		std::string name = STRFROM(emitter.nameID);

		ImGui::InputText("##name", &name);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			if (name.empty())
			{
				name = ("Emitter: " + std::to_string(i));
			}

			emitter.nameID = STRID(name);
		}

		ImGui::DragFloat3("Offset", glm::value_ptr(emitter.offset), 0.1f);

		ImGui::Separator();

		ImGui::DragFloat("Initial Velocity", &emitter.initialVelocity, 0.1f);
		ImGui::DragFloat("Initial Angular Velocity", &emitter.initialAngularVelocity, 0.1f);
		ImGui::DragFloat("Initial Acceleration", &emitter.initialAcceleration, 0.1f);
		ImGui::DragFloat("Drag", &emitter.drag, 0.01f);

		ImGui::DragFloat("Initial Lifetime", &emitter.initialLifetime, 0.1f, 0.01f, 100.0f);

		ImGui::DragFloat2("Initial Size", glm::value_ptr(emitter.initialSize), 0.05f);
		ImGui::ColorEdit4("Initial Color", glm::value_ptr(emitter.initialColor));

		ImGui::DragFloat("Initial Rotation", &emitter.initialRotation, 1.0f, -360.0f, 360.0f);

		//ImGui::DragFloat("Burst Count", &emitter.burstCount, 1.0f, 0.0f, 1000.0f);

		SpriteSource* spriteSource = spritesrcManager->Get(emitter.spriteSourceID);
		
		if (spriteSource)
		{

			ImGui::DragInt("Rows", &spriteSource->Rows(), 0.5f, 1);
			ImGui::DragInt("Columns", &spriteSource->Cols(), 0.5f, 1);

			ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(emitter.initialFrame)), 1, 0, spriteSource->GetFrameCount());


			if (spriteSource->GetTexture())
			{
				ImGui::Text("Texture\t"); ImGui::Image((void*)(__int64)spriteSource->GetTexture()->GetID(), ImVec2(125, 100), { 0, -1 }, { 1, 0 }, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				ImGui::SetCursorPos({ ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 100 }); // Move the cursor back to the position of the image
				if (ImGui::InvisibleButton("ImageButton", ImVec2(125, 100)))
				{
					ImGui::OpenPopup("TexturePopup");
				}
			}

		}
		else
		{
			if (ImGui::Button("Select texture", ImVec2(125, 100)))
				ImGui::OpenPopup("TexturePopup");
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_FILE"))
			{
				std::string filename((const char*)payload->Data);

				if (!filename.empty())
				{
					spritesrcManager->Has(filename) ? spritesrcManager->Get(filename) : spritesrcManager->Create(filename, 1, 1);
					emitter.spriteSourceID = STRID(filename);
				}

			}
			ImGui::EndDragDropTarget();
		}



		if (ImGui::BeginPopup("TexturePopup"))
		{
			ImGui::SetNextWindowSize(ImVec2(125, 100), ImGuiCond_FirstUseEver);
			// Begin a child window to make it scrollable
			ImGui::BeginChild("TextureList", ImVec2(125, 200), true, ImGuiWindowFlags_AlwaysUseWindowPadding);


			if (ImGui::Selectable("⨯ None"))
			{
				emitter.spriteSourceID = 0;
				ImGui::CloseCurrentPopup();
			}

			ImGui::Separator();

			const auto& componentsNames = texureManager->GetResourceNames();

			for (const auto& name : componentsNames)
			{
				auto texture = texureManager->Get(name);
				if (texture)
				{
					ImGui::Text("%s :", name.c_str());
					if (ImGui::ImageButton((void*)(__int64)texture->GetID(), ImVec2(75, 50), { 0, -1 }, { 1, 0 }))
					{
						spritesrcManager->Has(name) ? spritesrcManager->Get(name) : spritesrcManager->Create(name, 1, 1);
						emitter.spriteSourceID = STRID(name);
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndChild(); // End the child window
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}



	void ParticleSystem::DrawForcesConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::Text("Vortex");
		ImGui::DragFloat3("Vortex Center", glm::value_ptr(emitter.vortexCenter), 0.1f);
		ImGui::DragFloat("Vortex Strength", &emitter.vortexStrength, 0.1f);

		ImGui::Text("Wind");
		ImGui::DragFloat3("Wind Direction", glm::value_ptr(emitter.windDirection), 0.01f);
		ImGui::DragFloat("Wind Strength", &emitter.windStrength, 0.1f);

		ImGui::Text("Attractor");
		ImGui::DragFloat3("Attractor Position", glm::value_ptr(emitter.attractorPosition), 0.1f);
		ImGui::DragFloat("Attraction Strength", &emitter.attractionStrength, 0.1f);


		ImGui::Text("Gravity");
		ImGui::DragFloat3("Gravity", glm::value_ptr(emitter.gravity), 0.1f);
		ImGui::DragFloat("Gravity Scale", &emitter.gravityScale, 0.1f);

		ImGui::Text("Spin");
		ImGui::DragFloat3("Spin Offset", glm::value_ptr(emitter.spinCenter), 0.1f);
		ImGui::DragFloat("Spin Speed", &emitter.spinSpeed, 0.1f);

		ImGui::PopID();

	}

	void ParticleSystem::DrawSizeConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat2("Start Size", glm::value_ptr(emitter.startSize), 0.05f);
		ImGui::DragFloat2("End Size", glm::value_ptr(emitter.endSize), 0.05f);
		EasingCurveLib::DrawEditorUI(emitter.sizeEaseCurve, "Size Curve");

		ImGui::PopID();

	}


	void ParticleSystem::DrawRotationConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat("Start Rotation", &emitter.startRotation, 1.0f);
		ImGui::DragFloat("End Rotation", &emitter.endRotation, 1.0f);
		EasingCurveLib::DrawEditorUI(emitter.rotationEaseCurve, "Rotation Curve");

		ImGui::PopID();

	}

	void ParticleSystem::DrawFadeConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat("Start Fade", &emitter.startFade, 0.0f);
		ImGui::DragFloat("End Fade", &emitter.endFade, 1.0f);
		EasingCurveLib::DrawEditorUI(emitter.fadeEaseCurve, "Fade Curve");

		ImGui::PopID();

	}


	void ParticleSystem::DrawAnimationConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		if (ImGui::DragFloat("Anim Duration", &emitter.animDuration, 0.01f, 0.0001f, 10.0f))
			emitter.animDuration = std::max(0.0001f, emitter.animDuration);

		if (ImGui::DragInt("Frame Count", &emitter.animFrameCount, 1, 1, 100))
			emitter.animFrameCount = std::max(1, emitter.animFrameCount);

		bool loopChanged = ImGui::Checkbox("Loop", &emitter.loop);
		bool playOnceChanged = ImGui::Checkbox("Play Once", &emitter.playOnce);

		if (loopChanged && emitter.loop)
			emitter.playOnce = false;
		else if (playOnceChanged && emitter.playOnce)
			emitter.loop = false;

		ImGui::Checkbox("Reverse", &emitter.reverse);
		ImGui::Checkbox("Ping Pong", &emitter.pingPong);

		ImGui::DragFloat("Start Offset", &emitter.startOffset, 0.01f, 0.0f, 5.0f);

		ImGui::PopID();
	}


	void ParticleSystem::DrawShapeConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));
		const char* label = magic_enum::enum_name(emitter.spawnShapeData.shapeType).data();
		if (ImGui::BeginCombo("Shape", label)) {
			for (int s = 0; s < static_cast<int>(SpawnShape::SHAPES); ++s) {
				auto shape = static_cast<SpawnShape>(s);
				bool isSelected = (emitter.spawnShapeData.shapeType == shape);
				if (ImGui::Selectable(magic_enum::enum_name(shape).data(), isSelected)) {
					emitter.spawnShapeData.shapeType = shape;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		switch (emitter.spawnShapeData.shapeType) {
		case SpawnShape::LINE:
			ImGui::DragFloat3("Line Point 1", glm::value_ptr(emitter.spawnShapeData.data.line.p1), 0.1f);
			ImGui::DragFloat3("Line Point 2", glm::value_ptr(emitter.spawnShapeData.data.line.p2), 0.1f);
			break;
		case SpawnShape::CIRCLE:
			ImGui::DragFloat("Radius", &emitter.spawnShapeData.data.circle.radius, 0.1f);
			break;
		case SpawnShape::DONUT:
			ImGui::DragFloat("Inner Radius", &emitter.spawnShapeData.data.donut.innerRadius, 0.1f);
			ImGui::DragFloat("Outer Radius", &emitter.spawnShapeData.data.donut.outerRadius, 0.1f);
			break;
		case SpawnShape::RECT:
		{
			ImGui::DragFloat2("Center", glm::value_ptr(emitter.spawnShapeData.data.rect.center), 0.1f);
			ImGui::DragFloat2("Extent", glm::value_ptr(emitter.spawnShapeData.data.rect.extent), 0.1f);

			emitter.spawnShapeData.data.rect.extent = glm::max(emitter.spawnShapeData.data.rect.extent, glm::vec2(0.01f));
		}
		break;
		case SpawnShape::POINT:
			ImGui::DragFloat3("Point Position", glm::value_ptr(emitter.spawnShapeData.data.point.point), 0.1f);
			break;
		case SpawnShape::TEXTURE:
		{
			NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
			NTextureManager* textureManager = NTextureManager::Instance();

			std::string popupName = "TexturePopup##" + std::to_string(emitter.emitterID);

			ImGui::Text("Spawn Texture:");

			SpriteSource* spriteSource = spritesrcManager->Get(emitter.spawnShapeData.data.texture.spriteID);

			if (spriteSource && spriteSource->GetTexture())
			{

					ImGui::Image(
						(void*)(intptr_t)spriteSource->GetTexture()->GetID(),
						ImVec2(125, 100),
						ImVec2(0, -1), ImVec2(1, 0)); // flipped UVs for ImGui

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 100);

					if (ImGui::InvisibleButton("TextureButton", ImVec2(125, 100)))
						ImGui::OpenPopup(popupName.c_str());
				
			}
			else
			{
				if (ImGui::Button("Select Texture", ImVec2(125, 100)))
					ImGui::OpenPopup(popupName.c_str());
			}

			ImGui::DragFloat2("World Size", glm::value_ptr(emitter.spawnShapeData.data.texture.size), 0.1f);
			ImGui::SliderFloat("Invert Mask", &emitter.spawnShapeData.data.texture.invertMask, 0.0f, 1.0f);
			ImGui::DragFloat("Alpha Threshold", &emitter.spawnShapeData.data.texture.alphaThreshold, 0.01f, 0.0f, 1.0f);
			//ImGui::SameLine();
		/*	if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Minimum alpha value for spawn mask pixels");*/

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_FILE"))
				{
					std::string filename((const char*)payload->Data);
					if (!filename.empty())
					{
						spritesrcManager->Has(filename) ? spritesrcManager->Get(filename) : spritesrcManager->Create(filename, 1, 1);
						emitter.spawnShapeData.data.texture.spriteID = STRID(filename);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopup(popupName.c_str()))
			{
				ImGui::SetNextWindowSize(ImVec2(150, 200), ImGuiCond_FirstUseEver);
				ImGui::BeginChild("TextureList", ImVec2(150, 200), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

				if (ImGui::Selectable("⨯ None"))
				{
					emitter.spawnShapeData.data.texture.spriteID = 0;
					ImGui::CloseCurrentPopup();
				}
				ImGui::Separator();

				const auto& textureNames = textureManager->GetResourceNames();
				for (const auto& name : textureNames)
				{
					auto texture = textureManager->Get(name);
					if (texture)
					{
						ImGui::Text("%s", name.c_str());
						if (ImGui::ImageButton((void*)(intptr_t)texture->GetID(), ImVec2(75, 50), ImVec2(0, -1), ImVec2(1, 0))) // flip Y correctly
						{
							spritesrcManager->Has(name) ? spritesrcManager->Get(name) : spritesrcManager->Create(name, 1, 1);
							emitter.spawnShapeData.data.texture.spriteID = STRID(name);
							ImGui::CloseCurrentPopup();
						}
					}
				}

				ImGui::EndChild();
				ImGui::EndPopup();
			}

			break;
		}

		default:
			break;
		}
		ImGui::PopID();
	}


	bool CollapsingHeaderWithCheckbox(const std::string& label, bool* p_enabled)
	{
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();

		// Checkbox on the right
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
		ImGui::Checkbox(("##enabled" + label).c_str(), p_enabled);

		ImGui::SetCursorPosX(0);
		bool open = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::PopID();

		return open;
	}

	std::pair<bool, bool> DrawModifierHeader(const char* label, uint32_t& flags, uint32_t flagBit, int id)
	{
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		bool wasEnabled = (flags & flagBit) != 0;
		bool enabled = wasEnabled;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(7, 3));

		std::string checkboxLabel = "##Enable_" + std::string(label) + std::to_string(id);

		// Push text color
		ImVec4 textColor = (wasEnabled)
			? ImGui::GetStyleColorVec4(ImGuiCol_Text)
			: ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);

		// Push dimmed styles if disabled
		if (!wasEnabled)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.4f, 0.4f, 0.3f));
		}

		ImGui::Checkbox(checkboxLabel.c_str(), &enabled);
		ImGui::SameLine();

		bool opened = ImGui::TreeNodeEx(label,
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth);

		// Pop in reverse order
		if (!wasEnabled)
			ImGui::PopStyleColor(4);
		ImGui::PopStyleColor(); // text
		ImGui::PopStyleVar();

		// Set flag
		if (enabled) flags |= flagBit;
		else         flags &= ~flagBit;

		return { opened, enabled };
	}



	// Returns tuple: <isOpen, isEnabled, isRemoveClicked>
	std::tuple<bool, bool, bool> CollapsingHeaderWithCheckboxAndRemove(const std::string& label, bool* enabled)
	{
		bool remove = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImGui::PushID(label.c_str());

		// Checkbox
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));

		std::string checkboxLabel = "##" + label + "_enabled";
		ImGui::Checkbox(checkboxLabel.c_str(), enabled);
		ImGui::SameLine();

		// Collapsing header (take up remaining width except space for button)
		bool open = ImGui::TreeNodeEx(label.c_str(), flags);

		ImGui::PopStyleVar();


		// Place "X" button at right
		ImGui::SameLine(contentRegion.x - lineHeight);
		ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + contentRegion.x - lineHeight, cursorPos.y));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0, 0, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

		if (ImGui::Button(("X##" + label).c_str(), ImVec2(lineHeight, lineHeight)))
			remove = true;

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		return { open, *enabled, remove };
	}


	void ParticleSystem::ViewParticleSystemComponent(Entity& entity)
	{
		ParticleSystemComponent& psComp = entity.Get<ParticleSystemComponent>();


		uint8_t& flags = psComp.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("ParticleSystem", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<ParticleSystemComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();



		std::string name = STRFROM(psComp.nameID);

		ImGui::InputText("System Name", &name);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			if (name.empty())
			{
				name = "New System";
			}

			psComp.nameID = STRID(name);
		}


		if (ImGui::Button("Add Emitter"))
		{
			size_t startIndex = 0;
			for (size_t i = 0; i < psComp.emitterCount; ++i)
			{
				startIndex += psComp.emitters[i].maxParticles;
			}

			psComp.emitters[psComp.emitterCount].startIndex = startIndex;

			++psComp.emitterCount;

			NRegistry* registry = NRegistry::Instance();
			InitParticleBuffer(GetSystemEntities(), registry);
		}

		ImGui::Separator();

		for (size_t i = 0; i < psComp.emitterCount; ++i)
		{

			ImGui::PushID(static_cast<int>(i));


			ParticleEmitter& emitter = psComp.emitters[i];
			std::string label = "Emitter " + std::to_string(i) + ": " + STRFROM(emitter.nameID);

			{

				auto [opened, enabled, remove] = CollapsingHeaderWithCheckboxAndRemove(label, &emitter.enabled);


				//ImGui::SameLine();
		/*		std::string popOut = "Pop Out##" + std::to_string(i);
				if (ImGui::SmallButton(popOut.c_str()))
					emitter.openInWindow = true;*/

				if (opened)
				{
					ImGui::TreePop();


					if (!enabled)
						continue;


					//if (ImGui::CollapsingHeader("Initialization")) DrawPhysicsConfig(emitter, i);

					DrawInitConfig(emitter, i);

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));


					if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen)) DrawCoreSettings(emitter, i);

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Burst", &emitter.useBurst, i); open)
					{
						if (enabled)
						{
							ImGui::InputInt("Burst Count", &emitter.burstCount);
							ImGui::InputFloat("Burst Cooldown", &emitter.burstCooldown);
						}
						ImGui::TreePop();
					}

					if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen)) 
						DrawShapeConfig(emitter, i);

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					{
						auto [opened, enabled] = DrawModifierHeader("Lifetime", emitter.flags, PARTICLE_FLAG_RANDOMIZE_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawLifetimeSettings(emitter, i);
							ImGui::TreePop();
						}
					}

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));


					{
						auto [opened, enabled] = DrawModifierHeader("Animation", emitter.flags, PARTICLE_FLAG_ANIMATION, i);
						if (opened)
						{
							if (enabled)
								DrawAnimationConfig(emitter, i);
							ImGui::TreePop();
						}
					}

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					{
						auto [opened, enabled] = DrawModifierHeader("Size Over Lifetime", emitter.flags, PARTICLE_FLAG_SCALE_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawSizeConfig(emitter, i);
							ImGui::TreePop();
						}
					}

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));


					{
						auto [opened, enabled] = DrawModifierHeader("Rotation Over Lifetime", emitter.flags, PARTICLE_FLAG_ROTATION_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawRotationConfig(emitter, i);
							ImGui::TreePop();
						}
					}

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					{
						auto [opened, enabled] = DrawModifierHeader("Color Over Lifetime", emitter.flags, PARTICLE_FLAG_COLOR_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawColorConfig(emitter, i);
							ImGui::TreePop();
						}
					}
					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					{
						auto [opened, enabled] = DrawModifierHeader("Fade Over Lifetime", emitter.flags, PARTICLE_FLAG_ALPHA_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawFadeConfig(emitter, i);
							ImGui::TreePop();
						}
					}

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					{
						auto [opened, enabled] = DrawModifierHeader("Forces", emitter.flags, PARTICLE_FLAG_FORCES, i);
						if (opened)
						{
							if (enabled)
								DrawForcesConfig(emitter, i);
							ImGui::TreePop();
						}
					}

				}

				ImGui::PopID();


				if (remove)
				{
					
					for (size_t j = i; j < psComp.emitterCount; ++j)
					{
						psComp.emitters[j] = psComp.emitters[j + 1];
					}

					psComp.emitterCount--;

					NRegistry* registry = NRegistry::Instance();
					InitParticleBuffer(GetSystemEntities(), registry);

					--i;
					if (i < 0)
						break;
				}
			}


		}


		if (!enabled)
			ImGui::EndDisabled();


		ImGui::TreePop();
	}



	bool ParticleSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
			return true;

		InitParticleBuffer(entityList, registry);

		return true;
	}

	bool ParticleSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		m_TotalMaxParticles = 0;
		m_ParticleSSBO.AllocateParticleBuffer(m_TotalMaxParticles);
		return true;
	}

	bool ParticleSystem::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
		{
			NRegistry* registry = NRegistry::Instance();

			RemoveFromParticleBuffer(e.GetID(), registry);

		}

		return true;
	}

	bool ParticleSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			if (e.GetComponentID() == Component<ParticleSystemComponent>::GetID())
			{
				NRegistry* registry = NRegistry::Instance();

				InitParticleBuffer(entityList, registry);
			}
		return true;
	}

	bool ParticleSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			if (e.GetComponentID() == Component<ParticleSystemComponent>::GetID())
			{
				NRegistry* registry = NRegistry::Instance();

				InitParticleBuffer(entityList, registry);
			}

		return true;
	}

	void ParticleSystem::InitParticleBuffer(const std::vector<EntityID>& entityList, NRegistry* registry)
	{
		size_t totalNeeded = 0;
		for (const auto entityId : entityList)
		{
			ParticleSystemComponent& psComp = registry->GetComponent<ParticleSystemComponent>(entityId);
			for (size_t i = 0; i < psComp.emitterCount; i++)
			{
				ParticleEmitter& emitter = psComp.emitters[i];
				emitter.startIndex = totalNeeded;
				totalNeeded += emitter.maxParticles;
			}
		}

		if (totalNeeded != m_TotalMaxParticles || !m_ParticleSSBO.GetID())
		{
			m_TotalMaxParticles = totalNeeded;

			if (!m_ParticleSSBO.GetID())
				m_ParticleSSBO.GenerateBuffer();


			m_ParticleSSBO.AllocateParticleBuffer(m_TotalMaxParticles);

			dynamic_cast<ParticleBatchRenderer<Mesh>*>(m_Batcher)->SetSSBO(m_ParticleSSBO);


			// --- Dispatch clear shader here ---
			m_InitComputeShader->Bind();
			m_InitComputeShader->setInt("u_ParticleCount", static_cast<int>(m_TotalMaxParticles));
			m_ParticleSSBO.Bind(0); // bind SSBO to binding point 0

			const uint32_t workGroupSize = 256;
			uint32_t groups = (m_TotalMaxParticles + workGroupSize - 1) / workGroupSize;
			m_InitComputeShader->Dispatch(groups, 1, 1); // this should call glDispatchCompute internally
		}
	}
	void ParticleSystem::RemoveFromParticleBuffer(EntityID entityId, NRegistry* registry)
	{
		size_t totalNeeded = 0;

		ParticleSystemComponent& psComp = registry->GetComponent<ParticleSystemComponent>(entityId);
		for (size_t i = 0; i < psComp.emitterCount; i++)
		{
			ParticleEmitter& emitter = psComp.emitters[i];
			emitter.startIndex = totalNeeded;
			totalNeeded += emitter.maxParticles;
		}

			m_TotalMaxParticles -= totalNeeded;

			if (m_TotalMaxParticles < 0)
				m_TotalMaxParticles = 0;


			if (!m_ParticleSSBO.GetID())
				m_ParticleSSBO.GenerateBuffer();


			m_ParticleSSBO.AllocateParticleBuffer(m_TotalMaxParticles);

			dynamic_cast<ParticleBatchRenderer<Mesh>*>(m_Batcher)->SetSSBO(m_ParticleSSBO);


			// --- Dispatch clear shader here ---
			m_InitComputeShader->Bind();
			m_InitComputeShader->setInt("u_ParticleCount", static_cast<int>(m_TotalMaxParticles));
			m_ParticleSSBO.Bind(0); // bind SSBO to binding point 0

			const uint32_t workGroupSize = 256;
			uint32_t groups = (m_TotalMaxParticles + workGroupSize - 1) / workGroupSize;
			m_InitComputeShader->Dispatch(groups, 1, 1);

		
	}
}