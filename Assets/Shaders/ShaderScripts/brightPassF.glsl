#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform float threshold; // e.g. 1.0 or higher to pick bright areas only
uniform float intensity; // e.g. 1.0 or higher to pick bright areas only

void main()
{
    vec3 color = texture(inputs[0], v_TexCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // luminance calculation

    if (brightness > threshold)
        FragColor = vec4(color, 1.0)*intensity;
    else
        FragColor = vec4(0.0);
}
