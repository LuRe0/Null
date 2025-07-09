#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Easing.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

// Code adapted from https://easings.net/

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
#include <cmath>

    enum class EasingType
    {
        EaseInSine, EaseOutSine, EaseInOutSine,
        EaseInQuad, EaseOutQuad, EaseInOutQuad,
        EaseInCubic, EaseOutCubic, EaseInOutCubic,
        EaseInQuart, EaseOutQuart, EaseInOutQuart,
        EaseInQuint, EaseOutQuint, EaseInOutQuint,
        EaseInExpo, EaseOutExpo, EaseInOutExpo,
        EaseInCirc, EaseOutCirc, EaseInOutCirc,
        EaseInBack, EaseOutBack, EaseInOutBack,
        EaseInElastic, EaseOutElastic, EaseInOutElastic,
        EaseInBounce, EaseOutBounce, EaseInOutBounce, Linear,
        EaseInOutPeak,     
        EaseParabola,
        EASINGTYPES
    };

    class NLE_API Easing
    {
    public:
        static float Ease(float x, EasingType type)
        {
            switch (type)
            {
            case EasingType::EaseInSine:     return EaseInSine(x);
            case EasingType::EaseOutSine:    return EaseOutSine(x);
            case EasingType::EaseInOutSine:  return EaseInOutSine(x);
            case EasingType::EaseInQuad:     return EaseInQuad(x);
            case EasingType::EaseOutQuad:    return EaseOutQuad(x);
            case EasingType::EaseInOutQuad:  return EaseInOutQuad(x);
            case EasingType::EaseInCubic:    return EaseInCubic(x);
            case EasingType::EaseOutCubic:   return EaseOutCubic(x);
            case EasingType::EaseInOutCubic: return EaseInOutCubic(x);
            case EasingType::EaseInQuart:    return EaseInQuart(x);
            case EasingType::EaseOutQuart:   return EaseOutQuart(x);
            case EasingType::EaseInOutQuart: return EaseInOutQuart(x);
            case EasingType::EaseInQuint:    return EaseInQuint(x);
            case EasingType::EaseOutQuint:   return EaseOutQuint(x);
            case EasingType::EaseInOutQuint: return EaseInOutQuint(x);
            case EasingType::EaseInExpo:     return EaseInExpo(x);
            case EasingType::EaseOutExpo:    return EaseOutExpo(x);
            case EasingType::EaseInOutExpo:  return EaseInOutExpo(x);
            case EasingType::EaseInCirc:     return EaseInCirc(x);
            case EasingType::EaseOutCirc:    return EaseOutCirc(x);
            case EasingType::EaseInOutCirc:  return EaseInOutCirc(x);
            case EasingType::EaseInBack:     return EaseInBack(x);
            case EasingType::EaseOutBack:    return EaseOutBack(x);
            case EasingType::EaseInOutBack:  return EaseInOutBack(x);
            case EasingType::EaseInElastic:  return EaseInElastic(x);
            case EasingType::EaseOutElastic: return EaseOutElastic(x);
            case EasingType::EaseInOutElastic:return EaseInOutElastic(x);
            case EasingType::EaseInBounce:   return EaseInBounce(x);
            case EasingType::EaseOutBounce:  return EaseOutBounce(x);
            case EasingType::EaseInOutBounce:return EaseInOutBounce(x);
            case EasingType::EaseInOutPeak: return InOutPeak(x);
            case EasingType::EaseParabola:  return Parabola(x);
            default: return x; // linear fallback
            }
        }

    private:
        static constexpr float PI = 3.14159265358979323846f;

        static float InOutPeak(float x)
        {
            return x < 0.5f ? 2.0f * x : 2.0f * (1.0f - x);
        }

        static float Parabola(float x)
        {
            return 4.0f * x * (1.0f - x); // peaks at 0.5
        }

        static float EaseInSine(float x)
        {
            return 1 - std::cos((x * PI) / 2);
        }

        static float EaseOutSine(float x)
        {
            return std::sin((x * PI) / 2);
        }

        static float EaseInOutSine(float x)
        {
            return -(std::cos(PI * x) - 1) / 2;
        }

        static float EaseInQuad(float x)
        {
            return x * x;
        }

        static float EaseOutQuad(float x)
        {
            return 1 - (1 - x) * (1 - x);
        }

        static float EaseInOutQuad(float x)
        {
            return x < 0.5f ? 2 * x * x : 1 - std::pow(-2 * x + 2, 2) / 2;
        }

        static float EaseInCubic(float x)
        {
            return x * x * x;
        }

        static float EaseOutCubic(float x)
        {
            return 1 - std::pow(1 - x, 3);
        }

        static float EaseInOutCubic(float x)
        {
            return x < 0.5f ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
        }

        static float EaseInQuart(float x)
        {
            return x * x * x * x;
        }

        static float EaseOutQuart(float x)
        {
            return 1 - std::pow(1 - x, 4);
        }

        static float EaseInOutQuart(float x)
        {
            return x < 0.5f ? 8 * x * x * x * x : 1 - std::pow(-2 * x + 2, 4) / 2;
        }

        static float EaseInQuint(float x)
        {
            return x * x * x * x * x;
        }

        static float EaseOutQuint(float x)
        {
            return 1 - std::pow(1 - x, 5);
        }

        static float EaseInOutQuint(float x)
        {
            return x < 0.5f ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2;
        }

        static float EaseInExpo(float x)
        {
            return x == 0 ? 0 : std::pow(2, 10 * x - 10);
        }

        static float EaseOutExpo(float x)
        {
            return x == 1 ? 1 : 1 - std::pow(2, -10 * x);
        }

        static float EaseInOutExpo(float x)
        {
            if (x == 0) return 0;
            if (x == 1) return 1;
            return x < 0.5f ? std::pow(2, 20 * x - 10) / 2 : (2 - std::pow(2, -20 * x + 10)) / 2;
        }

        static float EaseInCirc(float x)
        {
            return 1 - std::sqrt(1 - x * x);
        }

        static float EaseOutCirc(float x)
        {
            return std::sqrt(1 - (x - 1) * (x - 1));
        }

        static float EaseInOutCirc(float x)
        {
            return x < 0.5f
                ? (1 - std::sqrt(1 - 4 * x * x)) / 2
                : (std::sqrt(1 - std::pow(-2 * x + 2, 2)) + 1) / 2;
        }

        static float EaseInBack(float x)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c3 = c1 + 1;
            return c3 * x * x * x - c1 * x * x;
        }

        static float EaseOutBack(float x)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c3 = c1 + 1;
            float t = x - 1;
            return 1 + c3 * t * t * t + c1 * t * t;
        }

        static float EaseInOutBack(float x)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c2 = c1 * 1.525f;
            if (x < 0.5f)
            {
                float t = 2 * x;
                return (t * t * ((c2 + 1) * t - c2)) / 2;
            }
            else
            {
                float t = 2 * x - 2;
                return (t * t * ((c2 + 1) * t + c2) + 2) / 2;
            }
        }

        static float EaseInElastic(float x)
        {
            constexpr float c4 = (2 * PI) / 3;
            if (x == 0) return 0;
            if (x == 1) return 1;
            return -std::pow(2, 10 * x - 10) * std::sin((x * 10 - 10.75f) * c4);
        }

        static float EaseOutElastic(float x)
        {
            constexpr float c4 = (2 * PI) / 3;
            if (x == 0) return 0;
            if (x == 1) return 1;
            return std::pow(2, -10 * x) * std::sin((x * 10 - 0.75f) * c4) + 1;
        }

        static float EaseInOutElastic(float x)
        {
            constexpr float c5 = (2 * PI) / 4.5f;
            if (x == 0) return 0;
            if (x == 1) return 1;
            if (x < 0.5f)
                return -(std::pow(2, 20 * x - 10) * std::sin((20 * x - 11.125f) * c5)) / 2;
            else
                return (std::pow(2, -20 * x + 10) * std::sin((20 * x - 11.125f) * c5)) / 2 + 1;
        }

        static float EaseInBounce(float x)
        {
            return 1 - EaseOutBounce(1 - x);
        }

        static float EaseOutBounce(float x)
        {
            constexpr float n1 = 7.5625f;
            constexpr float d1 = 2.75f;

            if (x < 1 / d1)
                return n1 * x * x;
            else if (x < 2 / d1)
                return n1 * (x - 1.5f / d1) * (x - 1.5f / d1) + 0.75f;
            else if (x < 2.5f / d1)
                return n1 * (x - 2.25f / d1) * (x - 2.25f / d1) + 0.9375f;
            else
                return n1 * (x - 2.625f / d1) * (x - 2.625f / d1) + 0.984375f;
        }

        static float EaseInOutBounce(float x)
        {
            if (x < 0.5f)
                return (1 - EaseOutBounce(1 - 2 * x)) / 2;
            else
                return (1 + EaseOutBounce(2 * x - 1)) / 2;
        }
    };


}