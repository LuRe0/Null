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
    vec2 _pad0; // align to 16-byte boundary
};


layout(std430, binding = 0) buffer ParticleBuffer
{
    ParticleInstance particles[];
};

uniform int u_ParticleCount;

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= uint(u_ParticleCount)) return;

    particles[id].alive = 0;
    particles[id].age = 0.0;
    particles[id].lifetime = 0.0;
    particles[id].scale = vec2(0.0);
    particles[id].velocity = vec3(0.0);
    particles[id].acceleration = vec3(0.0);
    particles[id].color = vec4(0.0, 0.0, 0.0, 0.0);
    particles[id].textureIndex = -1;
    particles[id].dimensions = vec2(1.0f);
}