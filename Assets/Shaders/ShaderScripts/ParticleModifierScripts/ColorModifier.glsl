// ColorModifier.glsl


uniform vec4  u_EmitterStartColor;
uniform vec4  u_EmitterEndColor;
uniform vec4  u_EmitterInitialColor;
uniform sampler1D u_ColorEaseCurve;

void Init_Color(inout ParticleInstance p, uint seed)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_COLOR_OVER_LIFETIME) == 0u)
    {
        p.color = u_EmitterInitialColor;
    }
    else
        p.color = u_EmitterStartColor;
}

void Update_Color(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_COLOR_OVER_LIFETIME) == 0u)
        return;
        
        
    float lifeRatio = clamp(p.age / p.lifetime, 0.0, 1.0);

    float colorEaseValue = texture(u_ColorEaseCurve, lifeRatio).r;

    p.color = mix(u_EmitterStartColor, u_EmitterEndColor, colorEaseValue);
}

void Exit_Color(inout ParticleInstance p, float lifeRatio)
{
}
