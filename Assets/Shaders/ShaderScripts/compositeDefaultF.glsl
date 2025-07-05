#version 430 core

uniform sampler2D inputs[2]; // [0] = Scene, [1] = Emissive
in vec2 v_TexCoord;
out vec4 FragColor;

void main() {
    vec4 base = texture(inputs[0], v_TexCoord);     // Scene
    vec4 emissive = texture(inputs[1], v_TexCoord); // Emissive

    FragColor = base + emissive; // Composite
    FragColor = clamp(base + emissive, 0.0, 1.0);
}