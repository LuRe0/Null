#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform vec2 texSize;
uniform vec2 texOffset;
uniform int entityID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec4 color;
flat out int vIntValue;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec2 adjustedCoords = vec2(aTexCoord.x/texSize.y, aTexCoord.y/texSize.x);

    TexCoord = texOffset + adjustedCoords;

    color = aColor;
    vIntValue = entityID;
}