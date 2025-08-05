#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EasingCurve.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Tools/Easing.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//

constexpr int kCurveSamples = 32;

namespace NULLENGINE
{
    enum EasingCurveFlags : uint8_t
    {
        EASING_FLAG_NONE = 0,
        EASING_FLAG_DIRTY = 1 << 0,
        EASING_FLAG_CUSTOM = 1 << 1,
        // Future: EASING_FLAG_LOOP = 1 << 2, etc.
    };

    struct EasingCurve
    {
        float values[kCurveSamples];
        EasingType type = EasingType::Linear;

        uint32_t bindlessHandle = 0;
        uint32_t textureID = 0;

        uint8_t flags = EASING_FLAG_DIRTY; // default to dirty
    };



    namespace EasingCurveLib
    {
        void Reset(EasingCurve& data);

        void GenerateFromType(EasingCurve& data);

        float Sample(const EasingCurve& data, float t);

        void UploadToGPU(EasingCurve& data);

        void BindToShader(EasingCurve& data, unsigned int shaderID, const char* uniformName, unsigned int textureUnit);

        void DrawEditorUI(EasingCurve& data, const char* label = "Easing Curve");

        // Flag helpers
        inline bool IsDirty(const EasingCurve& data) { return data.flags & EASING_FLAG_DIRTY; }
        inline void SetDirty(EasingCurve& data, bool value) { value ? data.flags |= EASING_FLAG_DIRTY : data.flags &= ~EASING_FLAG_DIRTY; }

        inline bool IsCustom(const EasingCurve& data) { return data.flags & EASING_FLAG_CUSTOM; }
        inline void SetCustom(EasingCurve& data, bool value) { value ? data.flags |= EASING_FLAG_CUSTOM : data.flags &= ~EASING_FLAG_CUSTOM; }

    }
}