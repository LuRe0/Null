// PhysicsModifier.glsl


uniform float u_EmitterInitialAcceleration;
uniform float u_EmitterInitialVelocity;
uniform float u_EmitterAngularVelocity;
uniform float u_Drag;


void Init_Physics(inout ParticleInstance p, vec3 dir)
{
    p.velocity = dir * u_EmitterInitialVelocity;
    p.acceleration = dir * u_EmitterInitialAcceleration;
}

void Update_Physics(inout ParticleInstance p)
{
    p.velocity += p.acceleration * u_DeltaTime;

    float d = clamp(u_Drag, 0.01, u_EmitterInitialVelocity);

    p.velocity *= 1.0 / (1.0 + u_DeltaTime * d);

    p.position += p.velocity * u_DeltaTime;

    p.rotation += u_EmitterAngularVelocity * u_DeltaTime;
}

void Exit_Physics(inout ParticleInstance p)
{
    
}
