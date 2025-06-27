// Source
//https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
// Supposebly from the NVidia Direct3D10 SDK
// Slightly modified for my purposes

// Utilities.glsl

uniform float u_Time;


// ---- Wang Hash ----
uint wangHash(uint seed) {
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return seed;
}

// ---- Xorshift ----
void xorshift(inout uint state) {
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
}

float GetRandomFloat(inout uint state) {
    xorshift(state);
    return float(state) / 4294967296.0; // Normalize to [0,1)
}

float GetRandomFloatRange(inout uint state, float minVal, float maxVal) {
    return mix(minVal, maxVal, GetRandomFloat(state));
}