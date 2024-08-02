#version 330 core
// Vertex attributes
layout(location = 0) in vec3 a_Position;         // Vertex position
layout(location = 1) in vec4 a_Color;            // Vertex color
layout(location = 2) in vec2 a_TexCoords;        // Vertex texture coordinates

// Instance attributes
layout(location = 3) in mat4 a_modelMatrix;      // Model matrix
layout(location = 7) in vec4 a_InstanceColor;    // Instance color
layout(location = 8) in vec2 a_InstanceTexCoords;// Instance texture coordinates
layout(location = 9) in vec2 a_InstanceTextSize; // Instance texture size
layout(location = 10) in int a_InstanceTexIndex; // Instance texture index
layout(location = 11) in uint a_EntityID;        // Instance entity ID

// Uniforms
uniform mat4 view;
uniform mat4 projection;

// Outputs to the fragment shader
out vec4 v_Color;
out vec4 v_Tint;
out vec2 v_TexCoord; 
flat out int v_TexIndex;
flat out uint v_EntityID;

void main()
{
    // Compute the final vertex position
    gl_Position = projection * view * a_modelMatrix * vec4(a_Position, 1.0);

    // Pass instance-specific attributes to the fragment shader
    
    vec2 adjustedCoords = vec2(a_TexCoords.x/a_InstanceTextSize.y, a_TexCoords.y/a_InstanceTextSize.x);

    v_TexCoord = adjustedCoords + a_InstanceTexCoords; // Or use a_InstanceTexCoords if that's what you intend
    v_Color = a_Color;
    v_Tint = a_InstanceColor;
    v_TexIndex = a_InstanceTexIndex;
    v_EntityID = a_EntityID;
}