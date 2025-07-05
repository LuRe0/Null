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


#include "ParticleModifierScripts/ParticleFlags.glsl"
#include "ParticleModifierScripts/Utilities.glsl"
#include "ParticleModifierScripts/ShapeModifier.glsl"
#include "ParticleModifierScripts/AnimationModifier.glsl"
#include "ParticleModifierScripts/AgeModifier.glsl"
#include "ParticleModifierScripts/SizeModifier.glsl"
#include "ParticleModifierScripts/PhysicsModifier.glsl"
#include "ParticleModifierScripts/ColorModifier.glsl"
#include "ParticleModifierScripts/VortexModifier.glsl"
#include "ParticleModifierScripts/RotationModifier.glsl"
#include "ParticleModifierScripts/FadeModifier.glsl"
#include "ParticleModifierScripts/AttractionModifier.glsl"
#include "ParticleModifierScripts/WindModifier.glsl"
#include "ParticleModifierScripts/GravityModifier.glsl"
#include "ParticleModifierScripts/RevolutionModifier.glsl"




// void onInit(inout ParticleInstance p, uint index)
// {
//     float seed = float(index) + float(u_StartIndex) * 1000.0;

//     p.alive = 1;
//     vec3 dir;
    
//     Init_Shape(p, seed, dir);
//     Init_Color(p, seed);
//     Init_Age(p, seed);
//     Init_Size(p, seed);
//     Init_Physics(p, dir);
// }


void onUpdate(inout ParticleInstance p)
{
    Update_Age(p);
    Update_Size(p);
    Update_Color(p);
    Update_Fade(p);
    Update_Animation(p);
    Update_Rotation(p);
    Update_Attraction(p);
    Update_Wind(p);
    Update_Vortex(p);
    Update_Gravity(p);
    Update_Gravity(p);
    Update_Revolution(p);
    Update_Physics(p);
}



void onExit(inout ParticleInstance p)
{
    p.alive = 0;
    p.age = 0.0;
    p.lifetime = 0.0;
    p.scale = vec2(0.0);
    p.velocity = vec3(0.0);
    p.acceleration = vec3(0.0);
    p.color = vec4(0.0, 0.0, 0.0, 0.0);
}

void main()
{
    uint localID = gl_GlobalInvocationID.x;
    if (localID >= u_MaxParticles) return;

    uint index = localID + u_StartIndex;

    // ParticleInstance p = particles[index];

    if (particles[index].alive == 1)
    {
        if (particles[index].age >= particles[index].lifetime)
        {
            onExit(particles[index]);
        }
        else
        {
            onUpdate(particles[index]);
        }
    }
}