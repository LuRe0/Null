// WindModifier.glsl

uniform vec3 u_WindDirection; // normalized
uniform float u_WindStrength;

void Update_Wind(inout ParticleInstance p)
{
    if ((u_EmitterFlags & PARTICLE_FLAG_FORCES) == 0u)
        return;
        
    // Simple noise for variation
    float noise = fract(sin(p.position.x * 12.9898 + u_Time * 78.233) * 43758.5453);
    
    vec3 windForce = u_WindDirection * u_WindStrength * noise;
    p.acceleration += windForce;
}
