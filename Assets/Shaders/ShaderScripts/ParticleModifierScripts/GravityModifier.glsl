// GravityModifier.glsl

uniform vec3 u_Gravity; // e.g., vec3(0.0, -9.81, 0.0)
uniform float u_GravityScale; // e.g., vec3(0.0, -9.81, 0.0)

void Update_Gravity(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;
        
    p.acceleration += u_Gravity * u_GravityScale;
}
