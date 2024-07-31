
// Per-vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

// Per-instance attributes (model matrix)
layout(location = 3) in mat4 aModelMatrix;
// Note: The matrix attribute takes 4 consecutive attribute locations (3, 4, 5, and 6)

// Per-instance attributes (color, texture coordinates, and texture index)
layout(location = 7) in vec4 aInstanceColor;
layout(location = 8) in vec2 aInstanceTexCoord;
layout(location = 9) in float aTextIndex;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Outputs to the fragment shader
out vec4 fragColor;
out vec2 fragTexCoord;
out float fragTextIndex;


void main()
{
    mat4 newModel = model * aModelMatrix;
    gl_Position = projection * view * newModel * vec4(aPos, 1.0);

    // Pass instance-specific attributes to the fragment shader
    fragColor = aInstanceColor;
    fragTexCoord = aTexCoord; // Alternatively, you can use aInstanceTexCoord if needed
    fragTextIndex = aTextIndex;
}