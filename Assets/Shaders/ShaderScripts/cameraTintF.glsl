#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform vec4 tintColor;
uniform float tintStrength;

void main()
{
    vec4 color = texture(inputs[0], v_TexCoord);
    FragColor = mix(color, tintColor, tintStrength);
}
