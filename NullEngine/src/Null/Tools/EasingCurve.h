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
	class NLE_API EasingCurve
	{
    public:
        EasingCurve();

        void DrawEditorUI(const char* label = "Easing Curve");
        float Sample(float t) const;
        void UploadToGPU(); // creates texture + bindless handle
        void GenerateFromType();
        void BindToShader(unsigned int shaderID, const char* uniformName, unsigned int textureUnit);

        unsigned int GetHandle() const { return m_BindlessHandle; }
        int GetType() const { return static_cast<int>(type); }
        bool IsDirty() const { return m_IsDirty; }

        float values[kCurveSamples]; // public for serialization or raw access

        EasingType type = EasingType::Linear;

    private:
        unsigned int m_TextureID = 0;
        unsigned int m_BindlessHandle = 0;
        bool m_IsDirty = true;
        bool m_IsCustom = false;

      
        //float rangeStart = 0.0f;
        //float rangeEnd = 1.0f;
	};

}