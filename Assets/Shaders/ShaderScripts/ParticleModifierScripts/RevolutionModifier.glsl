// VortexModifier.glsl

uniform vec3 u_SpinCenter;
uniform float u_SpinSpeed;  // angular speed, radians/sec or arbitrary units

void Update_Revolution(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;

    vec2 center = (u_EmitterOffset + u_ParentPosition + u_SpinCenter).xy;
    vec2 toParticle = p.position.xy - center;

    float distance = length(toParticle);
    if (distance < 0.001) return;

    vec2 tangent = normalize(vec2(-toParticle.y, toParticle.x));
    vec2 orbitVelocity = tangent * (u_SpinSpeed * distance);

    p.velocity.xy = orbitVelocity;

    // Lock to circle radius
    p.position.xy = center + normalize(toParticle) * distance;

    p.velocity.z = 0.0;
    p.position.z = 0.0;
}
