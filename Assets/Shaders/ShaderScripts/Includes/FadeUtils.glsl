float FadeSquare(vec3 localPos, float fade, float thickness) {
    float dist = 1.0f - max(abs(localPos.x), abs(localPos.y));
    float f = smoothstep(0.0, fade, dist);
    f *= smoothstep(thickness + fade, thickness, dist);
    return f;
}

float FadeCircle(vec3 localPos, float fade, float thickness) {
    float dist = 1.0f - length(localPos.xy);
    float f = smoothstep(0.0, fade, dist);
    f *= smoothstep(thickness + fade, thickness, dist);
    return f;
}

float FadeTriangle(vec3 localPos, float fade, float thickness) {
    vec2 A = vec2(-0.5, -0.5);
    vec2 B = vec2(0.5, -0.5);
    vec2 C = vec2(0.0, 0.5);
    float d1 = abs((B.y - A.y) * localPos.x - (B.x - A.x) * localPos.y + B.x * A.y - B.y * A.x) / length(B - A);
    float d2 = abs((C.y - B.y) * localPos.x - (C.x - B.x) * localPos.y + C.x * B.y - C.y * B.x) / length(C - B);
    float d3 = abs((A.y - C.y) * localPos.x - (A.x - C.x) * localPos.y + A.x * C.y - A.y * C.x) / length(A - C);
    float dist = min(min(d1, d2), d3);
    float f = smoothstep(0.0, fade, dist);
    f *= smoothstep(thickness + fade, thickness, dist);
    return f;
}

float ComputeFade(vec3 localPos, int fadeType, float fade, float thickness) {
    if (fadeType == 1) return FadeSquare(localPos, fade, thickness);
    if (fadeType == 2) return FadeTriangle(localPos, fade, thickness);
    if (fadeType == 3) return FadeCircle(localPos, fade, thickness);
    return 1.0;
}
