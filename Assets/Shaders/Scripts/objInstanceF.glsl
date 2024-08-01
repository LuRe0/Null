#version 330 core
layout (location = 0) out vec4 FragColor;     // Output color
layout (location = 1) out int FragColor1;     // Output color

// Inputs from the vertex shader
in vec4 fragColor;
in vec2 fragTexCoord;
flat in uint fragTextIndex;
flat in uint fragEntityID;


uniform sampler2D textures[64];


void main()
{
    // Sample the texture array using the texture coordinates and texture index
    vec4 texColor = texture(textures[fragTextIndex], fragTexCoord);

    // Combine the texture color with the instance color
    FragColor = texColor * fragColor;
	FragColor1 = int(fragEntityID);
}