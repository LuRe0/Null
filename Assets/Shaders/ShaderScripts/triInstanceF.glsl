#version 330 core

// Output to the framebuffer
layout (location = 0) out vec4 FragColor;     // Output color
layout (location = 1) out int FragID;     // Output color

// Inputs from the vertex shader
in vec4 v_Color;
in vec4 v_Tint;
in vec2 v_TexCoord;
in vec3 v_LocalPosition; 
in float v_Fade;
in float v_Thickness;
flat in int v_TexIndex;
flat in int v_EntityID;


uniform sampler2D u_Textures[32];
// uniform sampler2D u_Texture0;
// uniform sampler2D u_Texture1;

void main()
{
    // Example: Combine textures


    vec2 A = vec2(-0.5, -0.5); // Vertex A
    vec2 B = vec2(0.5, -0.5);  // Vertex B
    vec2 C = vec2(0.0, 0.5);   // Vertex C

    // Calculate distances from the fragment position to the edges of the triangle
    float distanceToEdge1 = abs((B.y - A.y) * v_LocalPosition.x - (B.x - A.x) * v_LocalPosition.y + B.x * A.y - B.y * A.x) / length(B - A);
    float distanceToEdge2 = abs((C.y - B.y) * v_LocalPosition.x - (C.x - B.x) * v_LocalPosition.y + C.x * B.y - C.y * B.x) / length(C - B);
    float distanceToEdge3 = abs((A.y - C.y) * v_LocalPosition.x - (A.x - C.x) * v_LocalPosition.y + A.x * C.y - A.y * C.x) / length(A - C);

    // Find the minimum distance to the edges
    float distance = min(min(distanceToEdge1, distanceToEdge2), distanceToEdge3);

    // Calculate the fade effect for the triangle
    float triangleFade = smoothstep(0.0, v_Fade, distance);
    triangleFade *= smoothstep(v_Thickness + v_Fade, v_Thickness, distance);

    int index = int(v_TexIndex);
    vec4 l_Texture;

    switch (index) 
    {
        case 0:  l_Texture = texture(u_Textures[0],  v_TexCoord); break;
        case 1:  l_Texture = texture(u_Textures[1],  v_TexCoord); break;
        case 2:  l_Texture = texture(u_Textures[2],  v_TexCoord); break;
        case 3:  l_Texture = texture(u_Textures[3],  v_TexCoord); break;
        case 4:  l_Texture = texture(u_Textures[4],  v_TexCoord); break;
        case 5:  l_Texture = texture(u_Textures[5],  v_TexCoord); break;
        case 6:  l_Texture = texture(u_Textures[6],  v_TexCoord); break;
        case 7:  l_Texture = texture(u_Textures[7],  v_TexCoord); break;
        case 8:  l_Texture = texture(u_Textures[8],  v_TexCoord); break;
        case 9:  l_Texture = texture(u_Textures[9],  v_TexCoord); break;
        case 10: l_Texture = texture(u_Textures[10], v_TexCoord); break;
        case 11: l_Texture = texture(u_Textures[11], v_TexCoord); break;
        case 12: l_Texture = texture(u_Textures[12], v_TexCoord); break;
        case 13: l_Texture = texture(u_Textures[13], v_TexCoord); break;
        case 14: l_Texture = texture(u_Textures[14], v_TexCoord); break;
        case 15: l_Texture = texture(u_Textures[15], v_TexCoord); break;
        case 16: l_Texture = texture(u_Textures[16], v_TexCoord); break;
        case 17: l_Texture = texture(u_Textures[17], v_TexCoord); break;
        case 18: l_Texture = texture(u_Textures[18], v_TexCoord); break;
        case 19: l_Texture = texture(u_Textures[19], v_TexCoord); break;
        case 20: l_Texture = texture(u_Textures[20], v_TexCoord); break;
        case 21: l_Texture = texture(u_Textures[21], v_TexCoord); break;
        case 22: l_Texture = texture(u_Textures[22], v_TexCoord); break;
        case 23: l_Texture = texture(u_Textures[23], v_TexCoord); break;
        case 24: l_Texture = texture(u_Textures[24], v_TexCoord); break;
        case 25: l_Texture = texture(u_Textures[25], v_TexCoord); break;
        case 26: l_Texture = texture(u_Textures[26], v_TexCoord); break;
        case 27: l_Texture = texture(u_Textures[27], v_TexCoord); break;
        case 28: l_Texture = texture(u_Textures[28], v_TexCoord); break;
        case 29: l_Texture = texture(u_Textures[29], v_TexCoord); break;
        case 30: l_Texture = texture(u_Textures[30], v_TexCoord); break;
        case 31: l_Texture = texture(u_Textures[31], v_TexCoord); break;
    }

    if(index >= 0)
        FragColor = l_Texture * v_Color * v_Tint;
    else
        FragColor = v_Color * v_Tint;

    FragColor.a *= triangleFade;

	FragID = v_EntityID;
}