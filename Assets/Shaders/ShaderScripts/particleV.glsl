#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

struct ParticleInstance {
    vec3 position;
    float _pad1;

    vec3 velocity;
    float _pad2;

    vec3 acceleration;
    float _pad3;

    vec4 color;

    float lifetime;
    float age;
    int alive;
    float _pad4;

    int textureIndex;
    vec2 texCoords;
    vec2 texSize;

    float scale;
};

// SSBO
layout(std430, binding = 0) buffer ParticleBuffer {
    ParticleInstance particleBuffer[];
};


// Uniforms
uniform mat4 view;
uniform mat4 projection;

// Out
out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TextureIndex;

void main()
{
    int particleIndex = gl_InstanceID;
    ParticleInstance p = particleBuffer[particleIndex];

    if (p.alive == 0) {
        gl_Position = vec4(0.0); // Cull dead particles
        v_TexCoord = vec2(0.0);
        v_Color = vec4(0.0);
        v_TextureIndex = -1;
        return;
    }

    vec3 worldPos = p.position + aPos * p.scale;
    gl_Position = projection * view * vec4(worldPos, 1.0);

    vec2 adjustedCoords = vec2(aTexCoord.x/p.texSize.y, aTexCoord.y/p.texSize.x);

    v_TexCoord = adjustedCoords + p.texCoords;

    v_Color = p.color * aColor;
    v_TextureIndex = p.textureIndex;
}
