#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform float grainAmount;
uniform float time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    vec4 color = texture(inputs[0], v_TexCoord);
    float grain = random(v_TexCoord * time) * grainAmount;
    FragColor = vec4(color.rgb + grain, color.a);
}
