// VortexModifier.glsl

uniform vec3 u_VortexCenter;
uniform float u_VortexStrength; // strength of swirling force

void Update_Vortex(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;

    vec2 center = (u_EmitterOffset + u_ParentPosition + u_VortexCenter).xy;
    vec2 toCenter = p.position.xy - center;

    float dist = length(toCenter);
    if (dist < 0.001) return;

    // Perpendicular direction in XY (for swirling)
    vec2 tangent = normalize(vec2(-toCenter.y, toCenter.x));

    // Swirling force, scaled by strength and distance
    vec2 vortexForce = tangent * u_VortexStrength;

    // Apply force to acceleration in XY
    p.acceleration.xy += vortexForce;

    // Optional: lock Z if in 2D
    p.acceleration.z = 0.0;
}
