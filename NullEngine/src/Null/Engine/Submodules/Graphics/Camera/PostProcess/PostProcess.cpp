
//------------------------------------------------------------------------------
//
// File Name:	PostProcess.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "PostProcess.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void PostProcess::Deserialize(const JSON& json)
    {
        if (!json.contains("postprocess"))
            return; // or handle error

        const JSON& ppJson = json["postprocess"];

        Enabled = ppJson.value("Enabled", true);

        UseBloom = ppJson.value("UseBloom", true);
        BloomThreshold = ppJson.value("BloomThreshold", 1.0f);
        BloomIntensity = ppJson.value("BloomIntensity", 0.8f);

        UseVignette = ppJson.value("UseVignette", false);
        VignetteRadius = ppJson.value("VignetteRadius", 0.75f);
        VignetteIntensity = ppJson.value("VignetteIntensity", 0.5f);

        UseGrayscale = ppJson.value("UseGrayscale", false);
        GrayscaleAmount = ppJson.value("GrayscaleAmount", 1.0f);

        UseTint = ppJson.value("UseTint", false);
        if (ppJson.contains("TintColor") && ppJson["TintColor"].is_array() && ppJson["TintColor"].size() == 4)
        {
            TintColor.r = ppJson["TintColor"][0].get<float>();
            TintColor.g = ppJson["TintColor"][1].get<float>();
            TintColor.b = ppJson["TintColor"][2].get<float>();
            TintColor.a = ppJson["TintColor"][3].get<float>();
        }
        TintStrength = ppJson.value("TintStrength", 0.5f);

        UseChromatic = ppJson.value("UseChromatic", false);
        ChromaticOffset = ppJson.value("ChromaticOffset", 1.5f);

        UseGrain = ppJson.value("UseGrain", false);
        GrainAmount = ppJson.value("GrainAmount", 0.2f);
    }

    void PostProcess::Serialize(JSON& json) const
    {
        // Create a nested object for postprocess data
        JSON ppJson;

        ppJson["Enabled"] = Enabled;

        ppJson["UseBloom"] = UseBloom;
        ppJson["BloomThreshold"] = BloomThreshold;
        ppJson["BloomIntensity"] = BloomIntensity;

        ppJson["UseVignette"] = UseVignette;
        ppJson["VignetteRadius"] = VignetteRadius;
        ppJson["VignetteIntensity"] = VignetteIntensity;

        ppJson["UseGrayscale"] = UseGrayscale;
        ppJson["GrayscaleAmount"] = GrayscaleAmount;

        ppJson["UseTint"] = UseTint;
        ppJson["TintColor"] = { TintColor.r, TintColor.g, TintColor.b, TintColor.a };
        ppJson["TintStrength"] = TintStrength;

        ppJson["UseChromatic"] = UseChromatic;
        ppJson["ChromaticOffset"] = ChromaticOffset;

        ppJson["UseGrain"] = UseGrain;
        ppJson["GrainAmount"] = GrainAmount;


        json["postprocess"] = ppJson;
    }
}
