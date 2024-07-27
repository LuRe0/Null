// Inputs from the vertex shader
in vec4 fragColor;
in vec2 fragTexCoord;
in float fragTextIndex;

// Output color
out vec4 FragColor;

// Texture array uniform
uniform sampler2DArray textures;

void main()
{
    // Sample the texture array using the texture coordinates and texture index
    vec4 texColor = texture(textures, vec3(fragTexCoord, fragTextIndex));

    // Combine the texture color with the instance color
    FragColor = texColor * fragColor;
}