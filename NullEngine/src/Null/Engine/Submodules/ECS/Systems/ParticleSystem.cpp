
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
#include <misc/cpp/imgui_stdlib.h>
#include "Null/Engine/Submodules/Events/IEvents.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/ParticleBatchRenderer.h"
#include <magic_enum/magic_enum.hpp>

#include <glad/glad.h> 


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


constexpr GLuint COLOR_EASE_TEXTURE_UNIT = 5;
constexpr GLuint SIZE_EASE_TEXTURE_UNIT = 6;
constexpr GLuint ROTATION_EASE_TEXTURE_UNIT = 7;
constexpr GLuint Fade_EASE_TEXTURE_UNIT = 8;

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	void SetShapeUniforms(ComputeShader& shader, const ParticleEmitter& emitter)
	{
		switch (emitter.shape)
		{
		case SpawnShape::POINT:
			shader.setVec2("u_Point", emitter.point.point);
			break;

		case SpawnShape::CIRCLE:
			shader.setFloat("u_CircleRadius", emitter.circle.radius);
			break;

		case SpawnShape::DONUT:
			shader.setFloat("u_DonutRadius1", emitter.donut.radius1);
			shader.setFloat("u_DonutRadius2", emitter.donut.radius2);
			break;

		case SpawnShape::LINE:
			shader.setVec3("u_LinePoint1", emitter.line.point1);
			shader.setVec3("u_LinePoint2", emitter.line.point2);
			break;

		case SpawnShape::RECT:
			shader.setVec4("u_Rect", glm::vec4(emitter.rect.x1, emitter.rect.y1, emitter.rect.x2, emitter.rect.y2));
			break;
		}

		shader.setBool("u_FollowParent", emitter.followParent);
	}


	ParticleSystem::ParticleSystem()
	{
		Require<TransformComponent>();
		Require<ParticleSystemComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<ParticleSystemComponent>(CreateParticleSystemComponent,
			[this](Entity& id) { this->ViewParticleSystemComponent(id); }, WriteParticleSystemComponent);

		m_TotalMaxParticles = 0;
		m_EmitComputeShader = 0;
		m_UpdateComputeShader = 0;
		m_InitComputeShader = 0;
	}

	void ParticleSystem::Load()
	{
		ISystem::Load();

		NComputeShaderManager* shaderMan = NEngine::Instance().Get<NComputeShaderManager>();
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
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &ParticleSystem::OnEntityCreated, eventManager, EventPriority::Low);

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();


		ParticleBatchRenderer<Mesh>* batcher = renderer->AddBatcher<ParticleBatchRenderer<Mesh>>("Particle");


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
		shader.setVec3("u_ParentPosition", transform.m_Translation);

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

		// lifetime
		shader.setFloat("u_EmitterMinLifetime", emitter.minLifetime);
		shader.setFloat("u_EmitterMaxLifetime", emitter.maxLifetime);
		shader.setInt("u_RandomizeLifetime", emitter.randomizeLifetime);

		// buffer offset
		shader.setInt("u_StartIndex", static_cast<int>(emitter.startIndex));

		// shape
		shader.setInt("u_EmitterShape", static_cast<int>(emitter.shape));

		// forces
		shader.setVec3("u_VortexCenter", emitter.vortexCenter);
		shader.setFloat("u_VortexStrength", emitter.vortexStrength);

		shader.setVec3("u_WindDirection", (glm::length(emitter.windDirection) > 0.001f) ? glm::normalize(emitter.windDirection) : emitter.windDirection);
		shader.setFloat("u_WindStrength", emitter.windStrength);

		shader.setVec3("u_AttractorPosition", emitter.AttractorPosition);
		shader.setFloat("u_AttractionStrength", emitter.AttractionStrength);

		shader.setVec3("u_SpinCenter", emitter.spinCenter);
		shader.setFloat("u_SpinSpeed", emitter.spinSpeed);

		shader.setVec3("u_Gravity", emitter.gravity);
		shader.setFloat("u_GravityScale", emitter.gravityScale);

		// any other uniforms...
		SetShapeUniforms(shader, emitter);
	}

	void ParticleSystem::Update(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		const GLuint workGroupSize = 256;

		for (const auto entityId : GetSystemEntities())
		{
			ParticleSystemComponent& particleSystem = registry->GetComponent<ParticleSystemComponent>(entityId);
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			for (ParticleEmitter& emitter : particleSystem.m_Emitters)
			{
				if (!emitter.enabled || emitter.finished)
					continue;

				// --- Pass 1: Update ---
				m_UpdateComputeShader->Bind();
				m_UpdateComputeShader->setFloat("u_DeltaTime", dt);
				m_UpdateComputeShader->setInt("u_MaxParticles", emitter.maxParticles);
				emitter.colorEaseCurve.BindToShader(m_UpdateComputeShader->GetID(), "u_ColorEaseCurve", COLOR_EASE_TEXTURE_UNIT);
				emitter.sizeEaseCurve.BindToShader(m_UpdateComputeShader->GetID(), "u_SizeEaseCurve", SIZE_EASE_TEXTURE_UNIT);
				emitter.rotationEaseCurve.BindToShader(m_UpdateComputeShader->GetID(), "u_RotationEaseCurve", ROTATION_EASE_TEXTURE_UNIT);
				emitter.rotationEaseCurve.BindToShader(m_UpdateComputeShader->GetID(), "u_AlphaEaseCurve", ROTATION_EASE_TEXTURE_UNIT);

				SetEmitterUniforms(*m_UpdateComputeShader, emitter, transform);

				m_ParticleSSBO.Bind(0);

				GLuint groups = (emitter.maxParticles + workGroupSize - 1) / workGroupSize;
				m_UpdateComputeShader->Dispatch(groups, 1, 1);

				// --- Pass 2: Emit ---
				emitter.emitAccumulator += dt * emitter.emitRate;
				int emitCount = (int)floor(emitter.emitAccumulator);
				emitter.emitAccumulator -= emitCount;

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

					if (emitter.spriteSource)
					{
						auto* tex = emitter.spriteSource->GetTexture();
						uint32_t compactID = NEngine::Instance()
							.Get<NTextureManager>()->GetTextureIndex(emitter.spriteSource->GetName());

						m_EmitComputeShader->setInt("u_EmitterTextureIndex",compactID);
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

	void ParticleSystem::RenderImGui()
	{
		ISystem::RenderImGui();
		ImGui::Text("Max Particles:", m_TotalMaxParticles);
	}

	void ParticleSystem::Render()
	{
		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();


		for (const auto entityId : GetSystemEntities())
		{
			ParticleSystemComponent& particleSystem = registry->GetComponent<ParticleSystemComponent>(entityId);
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			for (ParticleEmitter& emitter : particleSystem.m_Emitters)
			{
				if (!emitter.enabled || emitter.finished)
					continue;


				glm::vec4 worldPosition = transform.m_TransformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

				// Transform the world position to camera space
				glm::vec4 cameraSpacePosition = viewMatrix * worldPosition;

				// The depth is the z-component of the camera space position
				float depth = cameraSpacePosition.z;


				//model, mesh, spritesrc, tint, shadername, frameindex, entity
				renderer->AddParticleRenderCall(std::make_unique<ParticleData>(emitter.spriteSource, depth));
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


	void ParticleSystem::CreateParticleSystemComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();

		auto* comp = static_cast<ParticleSystemComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Name = jsonWrapper.GetString("name", "");

			if (json.contains("Emitters") && json["Emitters"].is_array())
			{
				for (const auto& emitterJson : json["Emitters"])
				{

					JsonReader emitterJsonWrapper(emitterJson);

					ParticleEmitter emitter;

					emitter.name = emitterJsonWrapper.GetString("name", "");
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

					glm::vec2 dimension = emitterJsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
					auto src = emitterJsonWrapper.GetString("texture", "");
					if (!src.empty())
						emitter.spriteSource = spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
					else
						emitter.spriteSource = nullptr;

					//size
					emitter.startSize = emitterJsonWrapper.GetVec2("startSize", glm::vec2(1.0f, 0.0f));
					emitter.endSize = emitterJsonWrapper.GetVec2("endSize", glm::vec2(1.0f, 0.0f));

					auto sizeCurve = emitterJsonWrapper.GetJSONObject("sizeCurve");
					emitter.sizeEaseCurve.type = static_cast<NULLENGINE::EasingCurve::CurveType>(sizeCurve.GetInt("type", 0));

					std::vector<float> svals = sizeCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.sizeEaseCurve.values), std::end(emitter.sizeEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < svals.size(); ++i)
						emitter.sizeEaseCurve.values[i] = svals[i];

					//color
					emitter.startColor = emitterJsonWrapper.GetVec4("startColor", glm::vec4(1.0f));
					emitter.endColor = emitterJsonWrapper.GetVec4("endColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

					auto colorCurve = emitterJsonWrapper.GetJSONObject("colorCurve");
					emitter.colorEaseCurve.type = static_cast<NULLENGINE::EasingCurve::CurveType>(colorCurve.GetInt("type", 0));

					std::vector<float> cvals = colorCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.colorEaseCurve.values), std::end(emitter.colorEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < cvals.size(); ++i)
						emitter.colorEaseCurve.values[i] = cvals[i];


					//rotation
					emitter.startRotation = emitterJsonWrapper.GetFloat("startRotation", 0.0f);
					emitter.endRotation = emitterJsonWrapper.GetFloat("endRotation", 0.0f);
					auto rotationCurve = emitterJsonWrapper.GetJSONObject("rotationCurve");
					emitter.rotationEaseCurve.type = static_cast<NULLENGINE::EasingCurve::CurveType>(colorCurve.GetInt("type", 0));

					std::vector<float> rvals = rotationCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.rotationEaseCurve.values), std::end(emitter.rotationEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < rvals.size(); ++i)
						emitter.rotationEaseCurve.values[i] = rvals[i];


					//fade
					emitter.startFade = emitterJsonWrapper.GetFloat("startFade", 0.0f);
					emitter.endFade = emitterJsonWrapper.GetFloat("endFade", 0.0f);

					auto fadeCurve = emitterJsonWrapper.GetJSONObject("fadeCurve");
					emitter.fadeEaseCurve.type = static_cast<NULLENGINE::EasingCurve::CurveType>(colorCurve.GetInt("type", 0));

					std::vector<float> fvals = fadeCurve.GetFloatArray("values", std::vector<float>(std::begin(emitter.fadeEaseCurve.values), std::end(emitter.fadeEaseCurve.values)));
					for (size_t i = 0; i < kCurveSamples && i < fvals.size(); ++i)
						emitter.fadeEaseCurve.values[i] = fvals[i];


					emitter.vortexCenter = emitterJsonWrapper.GetVec3("vortexCenter", glm::vec3(0.0f));
					emitter.vortexStrength = emitterJsonWrapper.GetFloat("vortexStrength", 0.0f);

					emitter.windDirection = emitterJsonWrapper.GetVec3("windDirection", glm::vec3(0.0f));
					emitter.windStrength = emitterJsonWrapper.GetFloat("windStrength", 0.0f);

					emitter.AttractorPosition = emitterJsonWrapper.GetVec3("attractorPosition", glm::vec3(0.0f));
					emitter.AttractionStrength = emitterJsonWrapper.GetFloat("attractionStrength", 0.0f);

					emitter.gravity = emitterJsonWrapper.GetVec3("gravity", glm::vec3(0.0f));
					emitter.gravityScale = emitterJsonWrapper.GetFloat("gravityScale", 0.0f);

					emitter.spinCenter = emitterJsonWrapper.GetVec3("spinCenter", glm::vec3(0.0f));
					emitter.spinSpeed = emitterJsonWrapper.GetFloat("spinSpeed", 0.0f);

					emitter.shape = static_cast<SpawnShape>(emitterJsonWrapper.GetInt("shape", static_cast<int>(SpawnShape::POINT)));

					switch (emitter.shape)
					{
					case SpawnShape::LINE:
						if (emitterJsonWrapper.HasData("line"))
						{
							auto lineObj = emitterJsonWrapper.GetJSONObject("line");
							emitter.line.point1.x = lineObj.GetFloat("point1_x", 0.0f);
							emitter.line.point1.y = lineObj.GetFloat("point1_y", 0.0f);
							emitter.line.point1.z = lineObj.GetFloat("point1_z", 0.0f);

							emitter.line.point2.x = lineObj.GetFloat("point2_x", 0.0f);
							emitter.line.point2.y = lineObj.GetFloat("point2_y", 0.0f);
							emitter.line.point2.z = lineObj.GetFloat("point2_z", 0.0f);
						}
						break;

					case SpawnShape::CIRCLE:
						if (emitterJsonWrapper.HasData("circle"))
						{
							auto circleObj = emitterJsonWrapper.GetJSONObject("circle");
							emitter.circle.radius = circleObj.GetFloat("radius", 1.0f);
						}
						break;

					case SpawnShape::DONUT:
						if (emitterJsonWrapper.HasData("donut"))
						{
							auto donutObj = emitterJsonWrapper.GetJSONObject("donut");
							emitter.donut.radius1 = donutObj.GetFloat("radius1", 0.5f);
							emitter.donut.radius2 = donutObj.GetFloat("radius2", 1.0f);
						}
						break;

					case SpawnShape::POINT:
						if (emitterJsonWrapper.HasData("point"))
						{
							auto pointObj = emitterJsonWrapper.GetJSONObject("point");
							emitter.point.point.x = pointObj.GetFloat("x", 0.0f);
							emitter.point.point.y = pointObj.GetFloat("y", 0.0f);
							emitter.point.point.z = pointObj.GetFloat("z", 0.0f);
						}
						break;

					case SpawnShape::RECT:
						if (emitterJsonWrapper.HasData("rect"))
						{
							auto rectObj = emitterJsonWrapper.GetJSONObject("rect");
							emitter.rect.x1 = rectObj.GetFloat("x1", 0.0f);
							emitter.rect.x2 = rectObj.GetFloat("x2", 1.0f);
							emitter.rect.y1 = rectObj.GetFloat("y1", 0.0f);
							emitter.rect.y2 = rectObj.GetFloat("y2", 1.0f);
						}
						break;
					default:
						break;
					}


					emitter.burstCount = emitterJsonWrapper.GetFloat("burstCount", 0.0f);

					comp->m_Emitters.push_back(emitter);
				}
			}
		}


		// Add or update component in registry
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		componentFactory->AddOrUpdate<ParticleSystemComponent>(id, comp, registry, comp->m_Name, comp->m_Emitters);
	}


	JSON ParticleSystem::WriteParticleSystemComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& psComp = *static_cast<ParticleSystemComponent*>(component);



		json["ParticleSystem"]["name"] = psComp.m_Name;

		// Emitters array
		nlohmann::json emittersJson = nlohmann::json::array();

		for (const auto& emitter : psComp.m_Emitters)
		{
			nlohmann::json e;

			e["name"] = emitter.name;
			e["emitterID"] = emitter.emitterID;

			e["texture"] = emitter.spriteSource != nullptr ? emitter.spriteSource->GetName() : "";
			e["dimension"] = emitter.spriteSource == nullptr
				? nlohmann::json::array({ 1, 1 })
				: nlohmann::json::array({ emitter.spriteSource->GetRows(), emitter.spriteSource->GetCols() });

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

			e["startSize"] = { emitter.startSize.x, emitter.startSize.y };
			e["endSize"] = { emitter.endSize.x, emitter.endSize.y };
			e["sizeCurve"]["type"] = emitter.sizeEaseCurve.GetType(); 
			e["sizeCurve"]["values"] = std::vector<float>(std::begin(emitter.sizeEaseCurve.values), std::end(emitter.sizeEaseCurve.values));

			e["startColor"] = { emitter.startColor.r, emitter.startColor.g, emitter.startColor.b, emitter.startColor.a };
			e["endColor"] = { emitter.endColor.r, emitter.endColor.g, emitter.endColor.b, emitter.endColor.a };
			e["colorCurve"]["type"] = emitter.colorEaseCurve.GetType(); 
			e["colorCurve"]["values"] = std::vector<float>(std::begin(emitter.colorEaseCurve.values), std::end(emitter.colorEaseCurve.values));

			e["startRotation"] = emitter.startRotation;
			e["endRotation"] = emitter.endRotation;
			e["rotationCurve"]["type"] = emitter.rotationEaseCurve.GetType(); 
			e["rotationCurve"]["values"] = std::vector<float>(std::begin(emitter.rotationEaseCurve.values), std::end(emitter.rotationEaseCurve.values));


			e["startFade"] = emitter.startFade;
			e["endFade"] = emitter.endFade;
			e["fadeCurve"]["type"] = emitter.fadeEaseCurve.GetType(); 
			e["fadeCurve"]["values"] = std::vector<float>(std::begin(emitter.fadeEaseCurve.values), std::end(emitter.fadeEaseCurve.values));

			e["vortexCenter"] = { emitter.vortexCenter.x, emitter.vortexCenter.y, emitter.vortexCenter.z };
			e["vortexStrength"] = emitter.vortexStrength;

			e["windDirection"] = { emitter.windDirection.x, emitter.windDirection.y, emitter.windDirection.z };
			e["windStrength"] = emitter.windStrength;

			e["attractorPosition"] = { emitter.AttractorPosition.x, emitter.AttractorPosition.y, emitter.AttractorPosition.z };
			e["attractionStrength"] = emitter.AttractionStrength;


			e["gravity"] = { emitter.gravity.x, emitter.gravity.y, emitter.gravity.z };
			e["gravityScale"] = emitter.gravityScale;

			e["spinCenter"] = { emitter.spinCenter.x, emitter.spinCenter.y, emitter.spinCenter.z };
			e["spinSpeed"] = emitter.spinSpeed;

			e["shape"] = static_cast<int>(emitter.shape);

			// Write each shape struct depending on current shape
			switch (emitter.shape)
			{
			case SpawnShape::LINE:
				e["line"]["point1_x"] = emitter.line.point1.x;
				e["line"]["point1_y"] = emitter.line.point1.y;
				e["line"]["point1_z"] = emitter.line.point1.z;

				e["line"]["point2_x"] = emitter.line.point2.x;
				e["line"]["point2_y"] = emitter.line.point2.y;
				e["line"]["point2_z"] = emitter.line.point2.z;
				break;

			case SpawnShape::CIRCLE:
				e["circle"]["radius"] = emitter.circle.radius;
				break;

			case SpawnShape::DONUT:
				e["donut"]["radius1"] = emitter.donut.radius1;
				e["donut"]["radius2"] = emitter.donut.radius2;
				break;

			case SpawnShape::POINT:
				e["point"]["x"] = emitter.point.point.x;
				e["point"]["y"] = emitter.point.point.y;
				e["point"]["z"] = emitter.point.point.z;
				break;

			case SpawnShape::RECT:
				e["rect"]["x1"] = emitter.rect.x1;
				e["rect"]["x2"] = emitter.rect.x2;
				e["rect"]["y1"] = emitter.rect.y1;
				e["rect"]["y2"] = emitter.rect.y2;
				break;

			default:
				break;
			}

			e["burstCount"] = emitter.burstCount;

			emittersJson.push_back(e);
		}

		json["ParticleSystem"]["Emitters"] = emittersJson;

		return json;
	}


	void ParticleSystem::DrawCoreSettings(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));
		ImGui::DragFloat("Emission Rate", &emitter.emitRate, 0.1f);
		if (ImGui::DragInt("Max Particles", reinterpret_cast<int*>(&emitter.maxParticles), 10, 1, 100000)) {
			NRegistry* registry = NEngine::Instance().Get<NRegistry>();
			InitParticleBuffer(GetSystemEntities(), registry);
		}
		ImGui::PopID();
	}

	void ParticleSystem::DrawColorConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::ColorEdit4("Start Color", glm::value_ptr(emitter.startColor));
		ImGui::ColorEdit4("End Color", glm::value_ptr(emitter.endColor));
		emitter.colorEaseCurve.DrawEditorUI();

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
		ImGui::DragFloat("Burst Count", &emitter.burstCount, 1.0f);

		ImGui::PopID();

	}


	void ParticleSystem::DrawInitConfig(ParticleEmitter& emitter, size_t i)
	{
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
		NTextureManager* texureManager = NEngine::Instance().Get<NTextureManager>();

		ImGui::PushID(static_cast<int>(i));

		ImGui::Text("Initial State");

		ImGui::Separator();

		ImGui::InputText("Name", &emitter.name);

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
		ImGui::DragInt("Initial Frame", &emitter.initialFrame, 1, 0, 100);

		ImGui::DragFloat("Burst Count", &emitter.burstCount, 1.0f, 0.0f, 1000.0f);

		
		if (emitter.spriteSource)
		{

			ImGui::DragInt("Rows", &emitter.spriteSource->Rows(), 0.5f, 1);
			ImGui::DragInt("Columns", &emitter.spriteSource->Cols(), 0.5f, 1);

			ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(emitter.initialFrame)), 1, 0, emitter.spriteSource->GetFrameCount());


			if (emitter.spriteSource->GetTexture())
			{
				ImGui::Text("Texture\t"); ImGui::Image((void*)(__int64)emitter.spriteSource->GetTexture()->GetID(), ImVec2(125, 100), { 0, -1 }, { 1, 0 }, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
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
					emitter.spriteSource = spritesrcManager->Has(filename) ? spritesrcManager->Get(filename) : spritesrcManager->Create(filename, 1, 1);

			}
			ImGui::EndDragDropTarget();
		}



		if (ImGui::BeginPopup("TexturePopup"))
		{
			ImGui::SetNextWindowSize(ImVec2(125, 100), ImGuiCond_FirstUseEver);
			// Begin a child window to make it scrollable
			ImGui::BeginChild("TextureList", ImVec2(125, 200), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

			const auto& componentsNames = texureManager->GetResourceNames();

			for (const auto& name : componentsNames)
			{
				auto texture = texureManager->Get(name);
				if (texture)
				{
					ImGui::Text("%s :", name.c_str());
					if (ImGui::ImageButton((void*)(__int64)texture->GetID(), ImVec2(75, 50), { 0, -1 }, { 1, 0 }))
					{
						emitter.spriteSource = spritesrcManager->Has(name) ? spritesrcManager->Get(name) : spritesrcManager->Create(name, 1, 1);
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
		ImGui::DragFloat3("Attractor Position", glm::value_ptr(emitter.AttractorPosition), 0.1f);
		ImGui::DragFloat("Attraction Strength", &emitter.AttractionStrength, 0.1f);


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
		emitter.sizeEaseCurve.DrawEditorUI();

		ImGui::PopID();

	}


	void ParticleSystem::DrawRotationConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat("Start Rotation", &emitter.startRotation, 1.0f);
		ImGui::DragFloat("End Rotation", &emitter.endRotation, 1.0f);

		emitter.rotationEaseCurve.DrawEditorUI();

		ImGui::PopID();

	}

	void ParticleSystem::DrawFadeConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));

		ImGui::DragFloat("Start Fade", &emitter.startFade, 0.0f);
		ImGui::DragFloat("End Fade", &emitter.endFade, 1.0f);

		emitter.rotationEaseCurve.DrawEditorUI();

		ImGui::PopID();

	}


	void ParticleSystem::DrawShapeConfig(ParticleEmitter& emitter, size_t i)
	{
		ImGui::PushID(static_cast<int>(i));
		const char* label = magic_enum::enum_name(emitter.shape).data();
		if (ImGui::BeginCombo("Shape", label)) {
			for (int s = 0; s < static_cast<int>(SpawnShape::SHAPES); ++s) {
				auto shape = static_cast<SpawnShape>(s);
				bool isSelected = (emitter.shape == shape);
				if (ImGui::Selectable(magic_enum::enum_name(shape).data(), isSelected)) {
					emitter.shape = shape;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		switch (emitter.shape) {
		case SpawnShape::LINE:
			ImGui::DragFloat3("Line Point 1", glm::value_ptr(emitter.line.point1), 0.1f);
			ImGui::DragFloat3("Line Point 2", glm::value_ptr(emitter.line.point2), 0.1f);
			break;
		case SpawnShape::CIRCLE:
			ImGui::DragFloat("Radius", &emitter.circle.radius, 0.1f);
			break;
		case SpawnShape::DONUT:
			ImGui::DragFloat("Inner Radius", &emitter.donut.radius1, 0.1f);
			ImGui::DragFloat("Outer Radius", &emitter.donut.radius2, 0.1f);
			break;
		case SpawnShape::RECT:
			ImGui::DragFloat("X Min", &emitter.rect.x1, 0.1f);
			ImGui::DragFloat("X Max", &emitter.rect.x2, 0.1f);
			ImGui::DragFloat("Y Min", &emitter.rect.y1, 0.1f);
			ImGui::DragFloat("Y Max", &emitter.rect.y2, 0.1f);
			break;
		case SpawnShape::POINT:
			ImGui::DragFloat3("Point Position", glm::value_ptr(emitter.point.point), 0.1f);
			break;
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

		bool enabled = (flags & flagBit) != 0;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(7, 3));

		std::string checkboxLabel = "##Enable_" + std::string(label) + std::to_string(id);

		// Push a darker color if disabled
		ImVec4 textColor = (enabled)
			? ImGui::GetStyleColorVec4(ImGuiCol_Text)
			: ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Checkbox(checkboxLabel.c_str(), &enabled);
		ImGui::PopStyleColor();

		ImGui::SameLine();

		bool opened = ImGui::TreeNodeEx(label,
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth);

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
		ImGui::InputText("System Name", &psComp.m_Name);

		if (ImGui::Button("Add Emitter"))
		{
			size_t startIndex = 0;
			for (const auto& e : psComp.m_Emitters)
				startIndex += e.maxParticles;

			psComp.AddEmitter("NewEmitter", startIndex, 100);

			NRegistry* registry = NEngine::Instance().Get<NRegistry>();
			InitParticleBuffer(GetSystemEntities(), registry);
		}

		ImGui::Separator();

		for (size_t i = 0; i < psComp.m_Emitters.size(); ++i)
		{

			ImGui::PushID(static_cast<int>(i));


			ParticleEmitter& emitter = psComp.m_Emitters[i];
			std::string label = "Emitter " + std::to_string(i) + ": " + emitter.name;

			//if (!ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			//	continue;

	/*		if (!CollapsingHeaderWithCheckbox(label, &emitter.enabled))
				continue;*/

			{

				auto [opened, enabled, remove] = CollapsingHeaderWithCheckboxAndRemove(label, &emitter.enabled);

				if (opened)
				{
					ImGui::TreePop();


					if (!enabled)
						continue;


					//if (ImGui::CollapsingHeader("Initialization")) DrawPhysicsConfig(emitter, i);

					DrawInitConfig(emitter, i);

					if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen)) DrawCoreSettings(emitter, i);
					if (ImGui::CollapsingHeader("Shape")) DrawShapeConfig(emitter, i);

					{

						auto [opened, enabled] = DrawModifierHeader("Lifetime", emitter.flags, PARTICLE_FLAG_RANDOMIZE_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawLifetimeSettings(emitter, i);

							ImGui::TreePop();
						}
					}

					{
						auto [opened, enabled] = DrawModifierHeader("Size Over Lifetime", emitter.flags, PARTICLE_FLAG_SCALE_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawSizeConfig(emitter, i);

							ImGui::TreePop();
						}
					}

					{
						auto [opened, enabled] = DrawModifierHeader("Rotation Over Lifetime", emitter.flags, PARTICLE_FLAG_ROTATION_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawRotationConfig(emitter, i);

							ImGui::TreePop();
						}
					}

					{
						auto [opened, enabled] = DrawModifierHeader("Color Over Lifetime", emitter.flags, PARTICLE_FLAG_COLOR_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawColorConfig(emitter, i);

							ImGui::TreePop();
						}
					}
					{
						auto [opened, enabled] = DrawModifierHeader("Fade Over Lifetime", emitter.flags, PARTICLE_FLAG_ALPHA_OVER_LIFETIME, i);
						if (opened)
						{
							if (enabled)
								DrawFadeConfig(emitter, i);

							ImGui::TreePop();
						}
					}
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
					psComp.m_Emitters.erase(psComp.m_Emitters.begin() + i);
					NRegistry* registry = NEngine::Instance().Get<NRegistry>();
					InitParticleBuffer(GetSystemEntities(), registry);
					--i;
					if (i < 0)
						break;
				}
			}


		}
	}



	bool ParticleSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) == entityList.end())
			return true;

		InitParticleBuffer(entityList, registry);

		return true;
	}

	void ParticleSystem::InitParticleBuffer(const std::vector<EntityID>& entityList, NRegistry* registry)
	{
		size_t totalNeeded = 0;
		for (const auto entityId : entityList)
		{
			ParticleSystemComponent& psComp = registry->GetComponent<ParticleSystemComponent>(entityId);
			for (ParticleEmitter& emitter : psComp.m_Emitters)
			{
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
}