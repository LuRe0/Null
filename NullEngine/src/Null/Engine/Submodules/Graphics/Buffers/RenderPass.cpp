
//------------------------------------------------------------------------------
//
// File Name:	RenderPass.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "RenderPass.h"
#include <magic_enum/magic_enum.hpp>	
#include "glad/glad.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void RenderPass::Load(JsonReader& reader)
    {
        NShaderManager* shaderManager = NEngine::Instance().Get<NShaderManager>();
        NFramebufferManager* fbMan = NEngine::Instance().Get<NFramebufferManager>();

        name = reader.GetString("name", "");
        clear = reader.GetBool("clear", true);
        clearColor = reader.GetVec4("clearColor", glm::vec4(0, 0, 0, 1));

        std::string shaderName = reader.GetString("shader", "");
        std::string framebufferName = reader.GetString("framebuffer", "");

        shader = shaderManager->Get(shaderName);
        framebuffer = framebufferName.empty() ? nullptr : fbMan->Get(framebufferName);

        stage = magic_enum::enum_cast<RenderStage>(reader.GetString("stage", "Main")).value_or(RenderStage::Opaque);
        blendMode = magic_enum::enum_cast<BlendMode>(reader.GetString("blendMode", "Alpha")).value_or(BlendMode::Alpha);
        cullMode = magic_enum::enum_cast<CullMode>(reader.GetString("cullMode", "Back")).value_or(CullMode::Back);

        // Depth-related fields:
        depthTest = reader.GetBool("depthTest", true);
        depthWrite = reader.GetBool("depthWrite", true);

        // Parse depthFunc, default GL_LESS
        std::string depthFuncStr = reader.GetString("depthFunc", "LESS");
        if (depthFuncStr == "LESS") depthFunc = GL_LESS;
        else if (depthFuncStr == "LEQUAL") depthFunc = GL_LEQUAL;
        else if (depthFuncStr == "EQUAL") depthFunc = GL_EQUAL;
        else if (depthFuncStr == "GREATER") depthFunc = GL_GREATER;
        else if (depthFuncStr == "GEQUAL") depthFunc = GL_GEQUAL;
        else if (depthFuncStr == "NOTEQUAL") depthFunc = GL_NOTEQUAL;
        else if (depthFuncStr == "ALWAYS") depthFunc = GL_ALWAYS;
        else if (depthFuncStr == "NEVER") depthFunc = GL_NEVER;
        else
        {
            NLE_CORE_WARN("Unknown depthFunc '{}', defaulting to LESS", depthFuncStr);
            depthFunc = GL_LESS;
        }

        if (reader.HasData("inputs"))
        {
            inputs.clear();
            auto inputArray = reader.GetArray("inputs");
            for (const auto& val : inputArray)
            {
                inputs.push_back(val.get<std::string>());
            }
        }

        if (reader.HasData("batchersToFlush"))
        {
            batchersToFlush.clear();
            auto inputArray = reader.GetArray("batchersToFlush");
            for (const auto& val : inputArray)
            {
                batchersToFlush.push_back(val.get<std::string>());
            }
        }
    }
}

