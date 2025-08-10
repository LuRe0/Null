#pragma once

//------------------------------------------------------------------------------
//
// File Name:	NAnimationEventManager.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Animation/AnimationEvent.h"

namespace NULLENGINE
{
    class NLE_API NAnimationEventManager : public NResourceManager < AnimationEvent, NAnimationEventManager >
    {
    public:

        void Load() override;

        void Init() override {}

        void Update(float dt) override {}

        void Unload() override;

        void Shutdown() override {}
    };

}
