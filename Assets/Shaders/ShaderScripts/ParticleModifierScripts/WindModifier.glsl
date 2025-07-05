// WindModifier.glsl

uniform vec3 u_WindDirection; // normalized
uniform float u_WindStrength;

void Update_Wind(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;

    uvec2 seedVec = floatBitsToUint(p.position.xy + u_Time * 0.5);
    uint seed = wangHash(seedVec.x ^ seedVec.y);

    float noise = GetRandomFloat(seed);

    vec2 swirl = vec2(
        GetRandomFloatRange(seed, -0.3, 0.3),
        GetRandomFloatRange(seed, -0.3, 0.3)
    );

    vec3 dir = normalize(u_WindDirection + vec3(swirl, 0.0));

    // Final wind force
    vec3 windForce = dir * u_WindStrength * noise;
    p.acceleration += windForce;
}
