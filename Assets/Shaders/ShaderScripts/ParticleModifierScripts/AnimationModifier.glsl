// AnimationModifier.glsl

uniform float u_AnimDuration;
uniform int u_AnimFrameCount;

uniform bool u_Looping;
uniform bool u_PlayOnce;
uniform bool u_Reverse;
uniform bool u_PingPong;
uniform float u_StartOffset; // usually randomized per-particle using seed

void Init_Animation(inout ParticleInstance p, float seed)
{

    if ((u_EmitterFlags & PARTICLE_FLAG_ANIMATION) == 0u)
        return;

    if (u_Reverse)
        p.frameIndex = uint(u_AnimFrameCount - 1);
    else
        p.frameIndex = 0u;

    p.animDirection = u_Reverse ? -1 : 1;
    p.animationTimer = u_AnimDuration / float(u_AnimFrameCount);
    p.startDelayTimer = fract(seed) * u_StartOffset;
}

void Update_Animation(inout ParticleInstance p)
{

    if ((u_EmitterFlags & PARTICLE_FLAG_ANIMATION) == 0u)
        return;
    // Wait for start delay
    if (p.startDelayTimer > 0.0)
    {
        p.startDelayTimer -= u_DeltaTime;
        return;
    }

    p.animationTimer -= u_DeltaTime;

    float frameTime = u_AnimDuration / float(u_AnimFrameCount);

    while (p.animationTimer <= 0.0)
    {
        p.animationTimer += frameTime;

        int nextFrame = int(p.frameIndex) + p.animDirection;

        bool atEnd = (nextFrame >= u_AnimFrameCount);
        bool atStart = (nextFrame < 0);

        if (atEnd || atStart)
        {
            if (u_PingPong)
            {
                p.animDirection *= -1;
                nextFrame = clamp(nextFrame, 0, u_AnimFrameCount - 1);
            }
            else if (u_Looping)
            {
                nextFrame = u_Reverse ? u_AnimFrameCount - 1 : 0;
            }
            else if (u_PlayOnce)
            {
                nextFrame = clamp(nextFrame, 0, u_AnimFrameCount - 1);
                p.animationTimer = 0.0;
                break;
            }
        }

        p.frameIndex = uint(nextFrame);
    }
}


void Exit_Animation(inout ParticleInstance p)
{

}
