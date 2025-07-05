#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	RenderPass.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Shader/Shader.h"
#include "Null/Engine/Submodules/Graphics/Buffers/Framebuffer.h"
#include "Null/Tools/JsonWrapper.h"

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

    enum class RenderStage {
        Opaque,
        Transparent,
        Emissive,
        PostProcess,
        UI,
        Debug,
        Final
    };
    enum class BlendMode { None, Alpha, Additive };
    enum class CullMode { None, Back, Front };

    struct NLE_API RenderPass
    {
        std::string name;
        std::vector<std::string> inputs;
        std::vector<std::string> batchersToFlush;
        Shader* shader = nullptr;
        Framebuffer* framebuffer = nullptr;
        glm::vec4 clearColor = { 0, 0, 0, 1 };
        bool clear = true;
        bool depthTest = true;
        bool depthWrite = true;
        unsigned int depthFunc = 0;
        RenderStage stage = RenderStage::Opaque;
        BlendMode blendMode = BlendMode::Alpha;
        CullMode cullMode = CullMode::Back;

        void Load(JsonReader& reader);
    };

}