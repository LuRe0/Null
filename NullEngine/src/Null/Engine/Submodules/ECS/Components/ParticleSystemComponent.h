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




    enum class SpawnShape { LINE, CIRCLE, DONUT, RECT, POINT, TEXTURE, SHAPES };

    struct lineEmit
    {
        glm::vec3 point1 = glm::vec3(0.0f);
        glm::vec3 point2 = glm::vec3(0.0f);
    };

    struct circleEmit
    {
        float radius = 50.0f;
    };

    struct donutEmit
    {
        float radius1 = 10.0f;
        float radius2 = 50.0f;
    };

    struct pointEmit
    {
        glm::vec3 point = glm::vec3(0.0f);
    };

    struct rectEmit
    {
        glm::vec2 center = glm::vec2(0.0f);
        glm::vec2 extent = glm::vec2(50.0f, 50.0f); // half width/height
    };

    struct textureEmit
    {
        SpriteSource* spriteSource = nullptr;
        glm::vec2 worldSize = glm::vec2(100.0f);
        float alphaThreshold = 0.5f;
        float invertMask = 0.0f;
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
        ParticleEmitter() = default;


        // Identification
        std::string name;
        uint32_t emitterID = 0;

        SpriteSource* spriteSource = nullptr;

        // Particle Pool
        unsigned int maxParticles = 1000;   
        unsigned int startIndex = 0;

        uint32_t bufferOffset = 0;

        bool enabled = true;

        // Emitter Lifecycle
        float emitRate = 10.0f;
        int emitCount = 0;
        float emitAccumulator = 10.0f;


        bool finished = false;

        // Particle Lifetime
        float minLifetime = 5.0f;
        float maxLifetime = 5.0f;
        bool randomizeLifetime = false;



        // Initial State
        glm::vec3 offset = glm::vec3(0.0f);
        float initialVelocity = 100.0f;
        float initialAngularVelocity = 100.0f;
        float initialAcceleration = 100.0f;
        float initialLifetime = 1.0f;
        glm::vec2 initialSize = glm::vec2(50.0f);
        glm::vec4 initialColor = glm::vec4(1.0f);
        float initialRotation = 0.0f;  // ← optional
        int initialFrame = 0;


        float drag = 0.001f;

        //vortex
        glm::vec3 vortexCenter = glm::vec3(0.0f);
        float vortexStrength = 1.0f;

        //wind
        glm::vec3 windDirection = glm::vec3(0.0f); // normalized
        float windStrength = 1.0f;

        //attractor
        glm::vec3 AttractorPosition = glm::vec3(0.0f);
        float AttractionStrength = 1.0f;;

        //gravity
        glm::vec3 gravity = glm::vec3(0.0f);
        float gravityScale = 1.0f;

        glm::vec3 spinCenter = glm::vec3(0.0f);
        float spinSpeed = 1.0f;

        // size
        glm::vec2 startSize = glm::vec2(50.0f);
        glm::vec2 endSize = glm::vec2(1.0f, 0.0f);
        EasingCurve sizeEaseCurve;


        //color
        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        EasingCurve colorEaseCurve;

        // Rotation
        float startRotation = 0.0f;
        float endRotation = 0.0f;
        EasingCurve rotationEaseCurve;


        //fade
        float startFade = 0.0f;
        float endFade = 1.0f;
        EasingCurve fadeEaseCurve;

        //Animation
        float animDuration = 1.0f;
        float startOffset = 0.0f; 
        int animFrameCount = 9;
        bool loop = true;
        bool playOnce = false;
        bool reverse = false;
        bool pingPong = false;

        // Spawn Shape
        SpawnShape shape = SpawnShape::POINT;
        //emitter shape
        lineEmit line;
        circleEmit circle;
        donutEmit donut;
        pointEmit point;
        rectEmit rect;
        textureEmit texture;

        bool followParent;
        bool openInWindow;


        // Burst
        bool useBurst = false;                
        int burstCount = 0;                   
        float burstCooldown = 0.0f;           
        int burstsRemaining = 0;              
        float burstTimer = 0.0f;              
        bool bursting = false;                


        // Flags
        uint32_t flags = PARTICLE_FLAG_NONE;


        ParticleEmitter(const std::string& n, size_t start, size_t count, uint32_t id = 0)
            : name(n), startIndex(start), maxParticles(count), emitterID(id), spriteSource(nullptr)
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