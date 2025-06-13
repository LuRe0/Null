#version 430 core

layout(local_size_x = 256) in;

struct Particle
{
    vec3 position;
    float pad1;

    vec3 velocity;
    float pad2;

    vec3 acceleration;
    float pad3;

    vec4 color;

    float lifetime;
    float age;
    int alive;
    float pad4;

    int textureIndex;
    vec3 pad5;
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

// Uniforms from CPU
uniform float u_DeltaTime;
uniform vec3  u_EmitterPosition;
uniform vec3  u_EmitterAcceleration;
uniform float u_EmitterLifetime;
uniform int   u_StartIndex;

void main()
{
    uint index = gl_GlobalInvocationID.x + uint(u_StartIndex);

    Particle p = particles[index];

    if (p.alive == 1)
    {
        p.age += u_DeltaTime;
        if (p.age >= p.lifetime)
        {
            p.alive = 0;
        }
        else
        {
            // Apply motion
            p.velocity += p.acceleration * u_DeltaTime;
            p.position += p.velocity * u_DeltaTime;
        }
    }
    else
    {
        // Example respawn logic (optional)
        // p.alive = 1;
        // p.age = 0.0;
        // p.position = u_EmitterPosition;
        // p.velocity = vec3(0.0);
        // p.acceleration = u_EmitterAcceleration;
        // p.lifetime = u_EmitterLifetime;
    }

    particles[index] = p;
}
