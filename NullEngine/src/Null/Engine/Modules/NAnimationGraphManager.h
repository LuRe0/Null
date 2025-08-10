#pragma once

//------------------------------------------------------------------------------
//
// File Name:	NAnimationGraphManager.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Animation/AnimationGraph.h"

namespace NULLENGINE
{
    class NLE_API NAnimationGraphManager : public NResourceManager < AnimationGraph, NAnimationGraphManager >
    {
    public:

        void Load() override;

        void SaveAllStateMachines();

        void Init() override {}

        void Update(float dt) override {}

        void Unload() override;

        void Shutdown() override {}
    };

}
