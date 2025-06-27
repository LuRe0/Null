#version 430 core

layout(local_size_x = 256) in;

struct ParticleInstance {
    vec3 position;
    float _pad1;

    vec3 velocity;
    float _pad2;

    vec3 acceleration;
    float _pad3;

    vec2 scale;
    float rotation;
    float _pad6;

    vec4 color;

    float lifetime;
    float age;
    int alive;
    float _pad4;

    int textureIndex;
    float _pad5[3];
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
}