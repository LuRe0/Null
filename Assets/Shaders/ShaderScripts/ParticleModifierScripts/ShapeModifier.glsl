// ShapeModifier.glsl

uniform vec3  u_EmitterOffset;
uniform vec3  u_ParentPosition;
uniform int   u_EmitterShape;       

uniform vec3 u_Point;
uniform float u_CircleRadius;
uniform float u_DonutRadius1;
uniform float u_DonutRadius2;
uniform vec3 u_LinePoint1;
uniform vec3 u_LinePoint2;
uniform vec2 u_RectCenter;
uniform vec2 u_RectExtent;

uniform sampler2D u_SpawnMask;
uniform vec2 u_MaskWorldSize;      // size in world units of mask rectangle
uniform float u_MaskAlphaThreshold;
uniform float u_InvertMask;


vec3 GenerateSpawnPosFromMask(uint seed)
{
    const int maxTries = 5;
    for (int i = 0; i < maxTries; ++i)
    {
        float rx = GetRandomFloatRange(seed, -u_MaskWorldSize.x * 0.5, u_MaskWorldSize.x * 0.5);
        float ry = GetRandomFloatRange(seed, -u_MaskWorldSize.y * 0.5, u_MaskWorldSize.y * 0.5);
        vec2 pos2D = vec2(rx, ry);

        vec2 uv = (pos2D / u_MaskWorldSize) + 0.5;
        uv.y = 1.0 - uv.y;

        float rawAlpha = texture(u_SpawnMask, uv).a;
        float alpha = mix(rawAlpha, 1.0 - rawAlpha, u_InvertMask);

        if (alpha > u_MaskAlphaThreshold)
        {
            return vec3(pos2D, 0.0);
        }
    }

    return vec3(0.0, 0.0, 0.0);
}

void Init_Shape(inout ParticleInstance p, uint seed, inout vec3 dir)
{
     // Convert uint to int and mix

    vec3 parentOffset = u_ParentPosition;
    vec3 pos;

    if (u_EmitterShape == 0) // LINE
    {
        vec3 pointOnLine = mix(u_LinePoint1, u_LinePoint2, GetRandomFloatRange(seed, 0.0, 1.0));
        pos = pointOnLine + u_EmitterOffset + parentOffset;
        dir = normalize(u_LinePoint2 - u_LinePoint1);
    }
    else if (u_EmitterShape == 1) // CIRCLE
    {
        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float radius = sqrt(GetRandomFloat(seed)) * u_CircleRadius;
        vec2 pos2D = vec2(cos(angle), sin(angle)) * radius;
        pos = vec3(pos2D, 0.0) + u_EmitterOffset + parentOffset;

        float a2 = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float z = GetRandomFloatRange(seed, -1.0, 1.0);
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(a2), r * sin(a2), z);
    }
    else if (u_EmitterShape == 2) // DONUT
    {
        vec2 result2D = vec2(
            GetRandomFloatRange(seed, -1.0, 1.0),
            GetRandomFloatRange(seed, -1.0, 1.0)
        );

        result2D = normalize(result2D);
        result2D *= GetRandomFloatRange(seed, u_DonutRadius1, u_DonutRadius2);

        pos = vec3(result2D, 0.0) + u_EmitterOffset + parentOffset;
        dir = vec3(result2D, 0.0);
    }
    else if (u_EmitterShape == 3) // RECT
    {
        float rx = GetRandomFloatRange(seed, -1.0, 1.0);
        float ry = GetRandomFloatRange(seed, -1.0, 1.0);

        vec2 localPos = vec2(rx * u_RectExtent.x, ry * u_RectExtent.y);
        pos = vec3(u_RectCenter + localPos, 0.0) + u_EmitterOffset + parentOffset;


        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853); // 0 to 2π
        float z = GetRandomFloatRange(seed, -1.0, 1.0);          // vertical component [-1,1]
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(angle), r * sin(angle), z);
    }
    else if (u_EmitterShape == 4) // POINT
    {
        pos = u_Point + u_EmitterOffset + parentOffset;
        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float z = GetRandomFloatRange(seed, -1.0, 1.0);
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(angle), r * sin(angle), z);
    }
    else if (u_EmitterShape == 5) // POINT
    {
        pos = GenerateSpawnPosFromMask(seed) + u_EmitterOffset + u_ParentPosition;

        // Generate direction as usual or something else
        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float z = GetRandomFloatRange(seed, -1.0, 1.0);
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(angle), r * sin(angle), z);
    }
    else // fallback
    {
        pos = u_EmitterOffset + parentOffset;

        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float z = GetRandomFloatRange(seed, -1.0, 1.0);
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(angle), r * sin(angle), z);
    }

    p.position = pos;
}

void Update_Shape(inout ParticleInstance p, float lifeRatio)
{
    // Empty or override for shape morphing if needed
}

void Exit_Shape(inout ParticleInstance p)
{
    // Empty or emit children if needed
}


