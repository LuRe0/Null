// FadeModifier.glsl

uniform float u_AlphaStart;             // 0.0 or 1.0
uniform float u_AlphaEnd;               // 1.0 or 0.0
uniform sampler1D u_AlphaEaseCurve;

void Init_Fade(inout ParticleInstance p, float seed)
{

}

void Update_Fade(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_ALPHA_OVER_LIFETIME) == 0u)
        return;

    float t = clamp(p.age / p.lifetime, 0.0, 1.0);
    float easedT = texture(u_AlphaEaseCurve, t).r;

    float alpha = mix(u_AlphaStart, u_AlphaEnd, easedT);

    p.color.a = alpha; // replaces only alpha
}

void Exit_Fade(inout ParticleInstance p)
{
    // Empty or emit children if needed
}
