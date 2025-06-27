// ShapeModifier.glsl

uniform bool u_RandomizeLifetime;
uniform float u_EmitterMinLifetime;
uniform float u_EmitterMaxLifetime;
uniform float u_EmitterInitialLifetime;


void Init_Age(inout ParticleInstance p, uint seed)
{

	if ((u_EmitterFlags & PARTICLE_FLAG_RANDOMIZE_LIFETIME) != 0u)
	{
		float rand = GetRandomFloatRange(seed, u_EmitterMinLifetime, u_EmitterMaxLifetime);
		p.lifetime = rand;
		p.age = 0;
	}
	else
	{
		p.lifetime = u_EmitterInitialLifetime;
		p.age = 0;
	}

    p.age = 0.0;
	
}

void Update_Age(inout ParticleInstance p)
{
    p.age += u_DeltaTime;
}

void Exit_Age(inout ParticleInstance p)
{
    // Empty or emit children if needed
}
