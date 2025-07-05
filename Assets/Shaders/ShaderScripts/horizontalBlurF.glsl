#version 430 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D inputs[1];

uniform float texelWidth;  // 1.0 / texture width
uniform float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = vec2(texelWidth, 0.0); // offset by 1 pixel horizontally
    vec3 result = texture(inputs[0], v_TexCoord).rgb * weights[0];

    for (int i = 1; i < 5; ++i)
    {
        result += texture(inputs[0], v_TexCoord + tex_offset * float(i)).rgb * weights[i];
        result += texture(inputs[0], v_TexCoord - tex_offset * float(i)).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0);
}
