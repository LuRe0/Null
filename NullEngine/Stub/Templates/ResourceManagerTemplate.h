#pragma once

//------------------------------------------------------------------------------
//
// File Name:	{{MANAGER_NAME}}.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Graphics/SpriteSource.h"

namespace NULLENGINE
{
    class NLE_API {{MANAGER_NAME}} : public NResourceManager < {{RESOURCE_TYPE}}, N{{MANAGER_NAME}} >
    {
    public:

        void Load() override;

        void Init() override {}

        void Update(float dt) override {}

        void Unload() override;

        void Shutdown() override {}
    };

}
