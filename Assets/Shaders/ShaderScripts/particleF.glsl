#version 430 core

uniform sampler2D u_Textures[31]; // 31 slots used for 2D textures
layout(binding = 31) uniform isampler1D  u_TextureIDToSlot;

flat in int v_TextureIndex;
in vec2 v_TexCoord;
in vec4 v_Color;

out vec4 FragColor;

void main()
{
    if (v_TextureIndex < 0) {
        FragColor = v_Color;
        
        if (FragColor.a < 0.1) discard;
    
        return;
    }

    // texelFetch returns vec4, take .r channel and cast to int explicitly
    int slotIndex = texelFetch(u_TextureIDToSlot, v_TextureIndex, 0).r; // returns correct int


    // Use slotIndex, not 'slot'
    vec4 texColor = texture(u_Textures[slotIndex], v_TexCoord);
    FragColor = texColor * v_Color;


    if (FragColor.a < 0.1) discard;
}
