#version 430 core

layout(local_size_x = 256) in;

struct ParticleInstance {
    vec3 position;
    float rotation;

    vec3 velocity;
    float lifetime;

    vec3 acceleration;
    float age;

    vec2 scale;
    vec2 dimensions;

    vec4 color;

    int alive;
    int textureIndex;
    uint frameIndex;
    int animDirection;

    float animationTimer;
    float startDelayTimer;
    vec2 _pad0;
};


layout(std430, binding = 0) buffer ParticleBuffer
{
    ParticleInstance particles[];
};

uniform float u_DeltaTime;
uniform int   u_StartIndex;
uniform int   u_MaxParticles;
uniform int   u_EmitCount;
uniform int   u_EmitterTextureIndex;
uniform vec2  u_Dimensions;
uniform int   u_EmitterInitialFrame;

#include "ParticleModifierScripts/ParticleFlags.glsl"
#include "ParticleModifierScripts/Utilities.glsl"
#include "ParticleModifierScripts/ShapeModifier.glsl"
#include "ParticleModifierScripts/AnimationModifier.glsl"
#include "ParticleModifierScripts/AgeModifier.glsl"
#include "ParticleModifierScripts/SizeModifier.glsl"
#include "ParticleModifierScripts/PhysicsModifier.glsl"
#include "ParticleModifierScripts/ColorModifier.glsl"
#include "ParticleModifierScripts/VortexModifier.glsl"
#include "ParticleModifierScripts/AttractionModifier.glsl"
#include "ParticleModifierScripts/WindModifier.glsl"
#include "ParticleModifierScripts/RotationModifier.glsl"
#include "ParticleModifierScripts/GravityModifier.glsl"




void onInit(inout ParticleInstance p, uint index)
{
    uint seed = wangHash(gl_GlobalInvocationID.x + uint(u_Time * 123.456));

    p.alive = 1;
    p.textureIndex = u_EmitterTextureIndex;
    p.dimensions = u_Dimensions;
    p.frameIndex = u_EmitterInitialFrame;
    vec3 dir;
    
    Init_Shape(p, seed, dir);
    Init_Animation(p, seed);
    Init_Color(p, seed);
    Init_Age(p, seed);
    Init_Rotation(p, seed);
    Init_Size(p, seed);
    Init_Physics(p, dir);
}


void main()
{
    uint id = gl_GlobalInvocationID.x;
    if (id >= u_EmitCount) return;

    for (uint i = 0; i < uint(u_MaxParticles); ++i)
    {
        uint index = u_StartIndex + ((id + i) % uint(u_MaxParticles));
        ParticleInstance p = particles[index];

        if (p.alive == 0)
        {
            onInit(p, index);
            particles[index] = p;
            break;
        }
    }
}