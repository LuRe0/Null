// AttractionModifier.glsl

uniform vec3 u_AttractorPosition;
uniform float u_AttractionStrength;

void Update_Attraction(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;

    vec3 center = u_EmitterOffset + u_ParentPosition + u_AttractorPosition;

    vec3 toAttractor = center - p.position;
    float dist = length(toAttractor);

    if (dist > 0.01)
    {
        vec3 dir = normalize(toAttractor);

        // --- Option 1: Soft falloff (recommended) ---
        // float forceMag = u_AttractionStrength / (dist + 1.0);

        // --- Option 2: Constant strength ---
        float forceMag = u_AttractionStrength * dist / (dist + 1.0);

        // --- Option 3: Eased falloff ---
        // float forceMag = u_AttractionStrength * (1.0 - smoothstep(0.0, 50.0, dist));

        p.acceleration += dir * forceMag;
    }
}
