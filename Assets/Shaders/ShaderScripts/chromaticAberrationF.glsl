#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform float offset;

void main()
{
    vec2 uv = v_TexCoord;
    vec2 redOffset = vec2(offset, 0.0);
    vec2 blueOffset = vec2(-offset, 0.0);

    float r = texture(inputs[0], uv + redOffset).r;
    float g = texture(inputs[0], uv).g;
    float b = texture(inputs[0], uv + blueOffset).b;

    FragColor = vec4(r, g, b, 1.0);
}
