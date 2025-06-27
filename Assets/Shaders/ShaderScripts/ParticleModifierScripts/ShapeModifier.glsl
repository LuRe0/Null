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
uniform vec4 u_Rect;
uniform bool u_FollowParent;

void Init_Shape(inout ParticleInstance p, uint seed, inout vec3 dir)
{
     // Convert uint to int and mix

    vec3 parentOffset = u_ParentPosition;
    vec3 pos;

    if (u_EmitterShape == 0) // POINT
    {
        pos = u_Point + u_EmitterOffset + parentOffset;
        float angle = GetRandomFloatRange(seed, 0.0, 6.2831853);
        float z = GetRandomFloatRange(seed, -1.0, 1.0);
        float r = sqrt(1.0 - z * z);
        dir = vec3(r * cos(angle), r * sin(angle), z);
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
    else if (u_EmitterShape == 3) // LINE
    {
        vec3 lineVec = u_LinePoint2 - u_LinePoint1;
        float t = GetRandomFloatRange(seed, 0.0, 1.0);
        vec3 pointOnLine = u_LinePoint1 + lineVec * t;
        pos = pointOnLine + u_EmitterOffset + parentOffset;
        dir = normalize(lineVec);
    }
    else if (u_EmitterShape == 4) // RECT
    {
        float x = GetRandomFloatRange(seed, u_Rect.x, u_Rect.z);
        float y = GetRandomFloatRange(seed, u_Rect.y, u_Rect.w);
        pos = vec3(x, y, 0.0) + u_EmitterOffset + parentOffset;

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
