#pragma once
#include "IComponent.h"
#include "Null/Tools/EasingCurve.h"

namespace NULLENGINE
{
    struct ParticleInstance
    {
        glm::vec3 position = glm::vec3(0.0f);      // 12
        float rotation = 0.0f;                     // 4  → now packed into the same vec4 block

        glm::vec3 velocity = glm::vec3(0.0f);      // 12
        float lifetime = 0.0f;                     // 4

        glm::vec3 acceleration = glm::vec3(0.0f);  // 12
        float age = 0.0f;                          // 4

        glm::vec2 scale = glm::vec2(1.0f);         // 8
        glm::vec2 dimensions = glm::vec2(1.0f);    // 8

        glm::vec4 color = glm::vec4(1.0f);         // 16

        int alive = 0;                             // 4
        int textureIndex = -1;                     // 4
        uint32_t frameIndex = 0;                   // 4
        int animDirection;

        float animationTimer;
        float startDelayTimer;
        glm::vec2 _pad0;
    };




    enum class SpawnShape : uint32_t { LINE, CIRCLE, DONUT, RECT, POINT, TEXTURE, SHAPES };

    struct LineEmit { glm::vec3 p1, p2; };
    struct CircleEmit { float radius; };
    struct DonutEmit { float innerRadius, outerRadius; };
    struct PointEmit { glm::vec3 point; };
    struct RectEmit { glm::vec2 center, extent; };
    struct TextureEmit { uint32_t spriteID; glm::vec2 size; float alphaThreshold; float invertMask; };

    union ShapeData
    {
        LineEmit line;
        CircleEmit circle;
        DonutEmit donut;
        PointEmit point;
        RectEmit rect;
        TextureEmit texture;
    };

    struct SpawnShapeData
    {
        SpawnShape shapeType = SpawnShape::POINT;
        ShapeData  data;
    };


    //Particle flags need to match glsl ones

    enum ParticleEmitterFlags : uint32_t
    {
        PARTICLE_FLAG_NONE = 0,

        PARTICLE_FLAG_ROTATION_OVER_LIFETIME = 1 << 0,
        PARTICLE_FLAG_SCALE_OVER_LIFETIME = 1 << 1,
        PARTICLE_FLAG_COLOR_OVER_LIFETIME = 1 << 2,
        PARTICLE_FLAG_VELOCITY_OVER_LIFETIME = 1 << 3,
        PARTICLE_FLAG_RANDOMIZE_LIFETIME = 1 << 4,


        PARTICLE_FLAG_FORCES = 1 << 5,

        PARTICLE_FLAG_CUSTOM_LOGIC = 1 << 6,
        PARTICLE_FLAG_COLLISION_ENABLED = 1 << 7,
        PARTICLE_FLAG_ALPHA_OVER_LIFETIME = 1 << 8,
        PARTICLE_FLAG_ANIMATION = 1 << 9
    };

    struct ParticleEmitter
    {
        // ─── Identification ─────────────────────────────────────────────────────────
        uint32_t nameID;
        uint32_t emitterID = 0;
        uint32_t spriteSourceID;
        uint32_t bufferOffset = 0;
        bool enabled = true;
        bool followParent = false;
        bool openInWindow = false;

        // ─── Pooling Info ───────────────────────────────────────────────────────────
        unsigned int maxParticles = 1000;
        unsigned int startIndex = 0;

        // ─── Emission ───────────────────────────────────────────────────────────────
        float emitRate = 10.0f;
        int emitCount = 0;
        float emitAccumulator = 0.0f;
        bool finished = false;

        // ─── Lifetime ───────────────────────────────────────────────────────────────
        float minLifetime = 5.0f;
        float maxLifetime = 5.0f;
        bool randomizeLifetime = false;

        // ─── Initial Properties ─────────────────────────────────────────────────────
        glm::vec3 offset = glm::vec3(0.0f);
        float initialLifetime = 1.0f;
        float initialVelocity = 100.0f;
        float initialAngularVelocity = 100.0f;
        float initialAcceleration = 100.0f;
        float initialRotation = 0.0f;
        int initialFrame = 0;
        glm::vec2 initialSize = glm::vec2(50.0f);
        glm::vec4 initialColor = glm::vec4(1.0f);
        float drag = 0.001f;

        // ─── Forces ─────────────────────────────────────────────────────────────────
        glm::vec3 vortexCenter = glm::vec3(0.0f);
        float vortexStrength = 1.0f;

        glm::vec3 windDirection = glm::vec3(0.0f);
        float windStrength = 1.0f;

        glm::vec3 attractorPosition = glm::vec3(0.0f);
        float attractionStrength = 1.0f;

        glm::vec3 gravity = glm::vec3(0.0f);
        float gravityScale = 1.0f;

        glm::vec3 spinCenter = glm::vec3(0.0f);
        float spinSpeed = 1.0f;

        // ─── Visual Transitions ─────────────────────────────────────────────────────
        glm::vec2 startSize = glm::vec2(50.0f);
        glm::vec2 endSize = glm::vec2(1.0f, 0.0f);
        EasingCurve sizeEaseCurve;

        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        EasingCurve colorEaseCurve;

        float startRotation = 0.0f;
        float endRotation = 0.0f;
        EasingCurve rotationEaseCurve;

        float startFade = 0.0f;
        float endFade = 1.0f;
        EasingCurve fadeEaseCurve;

        // ─── Animation ──────────────────────────────────────────────────────────────
        float animDuration = 1.0f;
        float startOffset = 0.0f;
        int animFrameCount = 9;
        bool loop = true;
        bool playOnce = false;
        bool reverse = false;
        bool pingPong = false;

        // ─── Spawn Shape ────────────────────────────────────────────────────────────
        SpawnShapeData spawnShapeData;

        // ─── Burst ──────────────────────────────────────────────────────────────────
        bool useBurst = false;
        int burstCount = 0;
        float burstCooldown = 0.0f;
        int burstsRemaining = 0;
        float burstTimer = 0.0f;
        bool bursting = false;

        // ─── Flags ──────────────────────────────────────────────────────────────────
        uint32_t flags = PARTICLE_FLAG_NONE;
    };


    //struct ParticleSystemComponent : public BaseComponent
    //{
    //    std::string m_Name;
    //    std::vector<ParticleEmitter> m_Emitters;

    //    ParticleSystemComponent(const std::string& inName = "DefaultSystem", const  std::vector<ParticleEmitter>& inEmitters = std::vector<ParticleEmitter>())
    //        : m_Name(inName)
    //        , m_Emitters(inEmitters)
    //    {}

    //    void AddEmitter(const std::string& emitterName, size_t particleStart, size_t particleCount)
    //    {
    //        m_Emitters.emplace_back(emitterName, particleStart, particleCount);
    //    }

    //    const std::string Name() const { return Component<ParticleSystemComponent>::TypeName(); }
    //    const uint32_t ID() const { return Component<ParticleSystemComponent>::GetID(); }
    //};


    constexpr size_t MaxEmittersPerSystem = 8;

    struct ParticleSystemComponent
    {
        std::array<ParticleEmitter, MaxEmittersPerSystem> emitters;
        uint32_t nameID;           // Stable ID for identifying system name type
        uint32_t emitterCount = 0;
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}