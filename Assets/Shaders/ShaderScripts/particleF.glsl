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
        return;
    }

    int slotIndex = texelFetch(u_TextureIDToSlot, v_TextureIndex, 0).r;
    vec4 texColor = texture(u_Textures[slotIndex], v_TexCoord);
    FragColor = texColor * v_Color;

    if(FragColor.a < 0.1) discard;

}
