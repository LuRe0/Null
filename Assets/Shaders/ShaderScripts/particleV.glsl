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

    vec2 scale;
    float rotation;
    float _pad6;

    vec4 color;

    float lifetime;
    float age;
    int alive;
    float _pad4;

    int textureIndex; // now compact ID
    float _pad5[3];
};

layout(std430, binding = 0) buffer ParticleBuffer {
    ParticleInstance particleBuffer[];
};

uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TextureIndex;

void main()
{
    int particleIndex = gl_InstanceID;
    ParticleInstance p = particleBuffer[particleIndex];

    if (p.alive == 0) {
        gl_Position = vec4(0.0);
        v_TexCoord = vec2(0.0);
        v_Color = vec4(0.0);
        v_TextureIndex = -1;
        return;
    }

    float angleRad = radians(p.rotation);
    mat2 rot = mat2(cos(angleRad), -sin(angleRad),
                    sin(angleRad),  cos(angleRad));

    vec2 rotated = rot * (aPos.xy * p.scale);
    vec3 worldPos = p.position + vec3(rotated, aPos.z);

    gl_Position = projection * view * vec4(worldPos, 1.0);

    //vec2 adjustedCoords = vec2(aTexCoord.x/p.texSize.y, aTexCoord.y/p.texSize.x);


    v_TexCoord = aTexCoord;
    v_Color = p.color * aColor;
    v_TextureIndex = p.textureIndex; // Already compact
}
