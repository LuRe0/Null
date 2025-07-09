#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	PostProcess.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

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
    using JSON = nlohmann::json;

    struct NLE_API PostProcess
    {
        bool Enabled = true;


        // Basic Effects and params:
        bool UseBloom = true;
        float BloomThreshold = 1.0f;
        float BloomIntensity = 0.8f;

        bool UseVignette = false;
        float VignetteRadius = 0.75f;
        float VignetteIntensity = 0.5f;

        bool UseGrayscale = false;
        float GrayscaleAmount = 1.0f;

        bool UseTint = false;
        glm::vec4 TintColor = glm::vec4(1.0f, 0.7f, 0.7f, 1.0f);
        float TintStrength = 0.5f;

        bool UseChromatic = false;
        float ChromaticOffset = 1.5f;

        bool UseGrain = false;
        float GrainAmount = 0.2f;

        void Deserialize(const JSON& json);

        void Serialize(JSON& json) const;
    };


}