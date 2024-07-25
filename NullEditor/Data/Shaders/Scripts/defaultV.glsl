#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform vec2 texOffset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = texOffset + aTexCoord;
    color = aColor;
}