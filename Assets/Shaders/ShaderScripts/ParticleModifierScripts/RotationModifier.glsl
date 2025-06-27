// RotationModifier.glsl


uniform float u_EmitterInitialRotation;
uniform float u_EmitterStartRotation;
uniform float u_EmitterEndRotation;
uniform sampler1D u_RotationEaseCurve;

void Init_Rotation(inout ParticleInstance p, uint seed)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_ROTATION_OVER_LIFETIME) == 0u)
       {
            p.rotation = u_EmitterInitialRotation;
            return;
       }
}


void Update_Rotation(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_ROTATION_OVER_LIFETIME) == 0u)
        return;

    float t = clamp(p.age / p.lifetime, 0.0, 1.0);

    float rotationEaseValue = texture(u_RotationEaseCurve, t).r;

    p.rotation = mix(u_EmitterStartRotation, u_EmitterEndRotation, rotationEaseValue);
}

void Exit_Rotation(inout ParticleInstance p)
{
    // Empty or emit children if needed
}
