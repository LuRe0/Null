#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform float radius;
uniform float intensity;

void main()
{
    vec2 uv = v_TexCoord - 0.5;
    float dist = length(uv) / radius;
    float vignette = clamp(1.0 - dist, 0.0, 1.0);
    
    vec4 color = texture(inputs[0], v_TexCoord);
    FragColor = mix(color * vignette, color, 1.0 - intensity);
}
