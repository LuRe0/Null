#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];
uniform float grayscaleAmount;

void main()
{
    vec4 color = texture(inputs[0], v_TexCoord);
    float gray = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    vec3 result = mix(color.rgb, vec3(gray), grayscaleAmount);
    FragColor = vec4(result, color.a);
}