// SizeModifier.glsl


uniform vec2  u_EmitterStartSize;
uniform vec2  u_EmitterEndSize;
uniform vec2  u_EmitterInitialSize;
uniform sampler1D u_SizeEaseCurve;

void Init_Size(inout ParticleInstance p, uint seed)
{
    // Only init size if flag is enabled
    if ((u_EmitterFlags & PARTICLE_FLAG_SCALE_OVER_LIFETIME) != 0u)
    {
        p.scale = u_EmitterStartSize;
    }
    else
        p.scale = u_EmitterInitialSize;

}

void Update_Size(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_SCALE_OVER_LIFETIME) != 0u)
    {
        float lifeRatio = clamp(p.age / p.lifetime, 0.0, 1.0);

        float sizeEaseValue = texture(u_SizeEaseCurve, lifeRatio).r;


        p.scale = mix(u_EmitterStartSize, u_EmitterEndSize, sizeEaseValue);
    }
}


void Exit_Size(inout ParticleInstance p, float lifeRatio)
{
}
