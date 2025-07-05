#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

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



layout(std430, binding = 0) buffer ParticleBuffer {
    ParticleInstance particleBuffer[];
};

uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TextureIndex;


vec2 GetUV(in ParticleInstance p)
{
    uint dimY = uint(p.dimensions.y);
    uint dimX = uint(p.dimensions.x);

    uint col = p.frameIndex % dimY;
    uint row = p.frameIndex / dimX;

    float u = (1.0 / p.dimensions.y) * float(col);
    float v = (1.0 / p.dimensions.x) * float(row);


    return vec2(u, v);
}

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

    // Rotate first, then scale to prevent distortion
    vec2 rotatedXY = (rot * aPos.xy) * p.scale;

    // Combine with Z and add to world position
    vec3 localOffset = vec3(rotatedXY, aPos.z);
    vec3 worldPos = p.position + localOffset;

    // Transform to clip space
    gl_Position = projection * view * vec4(worldPos, 1.0);


    vec2 adjustedCoords = vec2(aTexCoord.x/p.dimensions.y, aTexCoord.y/p.dimensions.x);

    vec2 uvs = GetUV(p);
    v_TexCoord = adjustedCoords + uvs;
    v_Color = p.color * aColor;
    v_TextureIndex = p.textureIndex; // Already compact
}
