#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct ParticleInstance
    {
        glm::vec3 position;
        float pad1;

        glm::vec3 velocity;
        float pad2;

        glm::vec3 acceleration;
        float pad3;

        glm::vec4 color;

        float lifetime;
        float age;
        int alive;
        float pad4;

        int textureIndex;
        float pad5[3];
    };


    enum class SpawnShape
    {
        Point,
        Sphere,
        Box,
        Cone
    };

    enum ParticleEmitterFlags : uint32_t
    {
        PARTICLE_FLAG_NONE = 0,
        PARTICLE_FLAG_LOOPING = 1 << 0,
        PARTICLE_FLAG_LOCAL_SPACE = 1 << 1,
        PARTICLE_FLAG_BURST_MODE = 1 << 2,
    };

    struct ParticleEmitter
    {
        ParticleEmitter() = default;
        // Core
        std::string name;
        uint32_t emitterID = 0;

        glm::vec3 position = glm::vec3(0.0f);
        float emissionRate = 10.0f; // particles per second

        float lifetime = 5.0f;      // per-particle lifetime
        float spawnTimer = 0.0f;

        size_t maxParticles = 1000;
        size_t startIndex = 0;      // offset into global particle buffer
        uint32_t bufferOffset = 0;  // for compute dispatch if needed

        uint32_t flags = PARTICLE_FLAG_NONE;
        bool enabled = true;

        // Duration & looping
        float emitterDuration = -1.0f; // -1 = infinite
        float emitterAge = 0.0f;
        bool finished = false;

        // Spawn config
        glm::vec3 initialVelocity = glm::vec3(100.0f);
        glm::vec3 acceleration = glm::vec3(100.0f); // gravity etc.

        glm::vec2 startSize = glm::vec2(1.0f, 0.0f); // startSize → endSize
        glm::vec2 endSize = glm::vec2(1.0f, 0.0f); // startSize → endSize
        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f); // fade-out

        // Rotation
        float startRotation = 0.0f;
        float endRotation = 0.0f;

        // Shape-based spawn
        SpawnShape shape = SpawnShape::Point;
        glm::vec3 shapeSize = glm::vec3(1.0f); // radius or box extents


        // Burst mode
        float burstCount = 0.0f;

        ParticleEmitter(const std::string& n, size_t start, size_t count, uint32_t id = 0)
            : name(n), startIndex(start), maxParticles(count), emitterID(id)
        {
        }
    };



    struct ParticleSystemComponent : public BaseComponent
    {
        std::string m_Name;
        std::vector<ParticleEmitter> m_Emitters;

        ParticleSystemComponent(const std::string& inName = "DefaultSystem", const  std::vector<ParticleEmitter>& inEmitters = std::vector<ParticleEmitter>())
            : m_Name(inName)
            , m_Emitters(inEmitters)
        {}

        void AddEmitter(const std::string& emitterName, size_t particleStart, size_t particleCount)
        {
            m_Emitters.emplace_back(emitterName, particleStart, particleCount);
        }

        const std::string Name() const { return Component<ParticleSystemComponent>::TypeName(); }
        const uint32_t ID() const { return Component<ParticleSystemComponent>::GetID(); }
    };
}