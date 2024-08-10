#pragma once
#define NWINDOW_H

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NIncludes.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Engine/Modules/NWindow.h"
#include "Null/Engine/Modules/NSceneManager.h"
#include "Null/Engine/Modules/NEventManager.h"
#include "Null/Engine/Modules/NRegistry.h"
#include "Null/Engine/Modules/NShaderManager.h"
#include "Null/Engine/Modules/NSpriteSourceManager.h"
#include "Null/Engine/Modules/NMeshManager.h"
#include "Null/Engine/Modules/NRenderer.h"
#include "Null/Engine/Modules/NCameraManager.h"
#include "Null/Engine/Modules/NTextureManager.h"
#include "Null/Engine/Modules/NComponentFactory.h"
#include "Null/Engine/Modules/NEntityFactory.h"
#include "Null/Engine/Modules/NScriptingInterface.h"
#include "Null/Engine/Submodules/ECS/Systems/PhysicsSystem.h"
#include "Null/Engine/Submodules/ECS/Systems/SpriteRenderSystem.h"
#include "Null/Engine/Submodules/ECS/Systems/TransformSystem.h"
#include "Null/Engine/Submodules/ECS/Systems/AnimationSystem.h"
#include "Null/Engine/Submodules/ECS/Systems/ScriptSystem.h"
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"
#include "Null/Engine/Submodules/ECS/Components/TransformComponent.h"
#include "Null/Engine/Submodules/ECS/Components/SpriteComponent.h"
#include "Null/Engine/Submodules/ECS/Components/TagComponent.h"
#include "Null/Engine/Submodules/ECS/Components/CameraComponent.h"
#include "Null/Engine/Submodules/ECS/Components/AnimationComponent.h"
#include "Null/Engine/Submodules/ECS/Components/ScriptComponent.h"
#include "Null/Engine/Submodules/ECS/Components/Rigidbody2DComponent.h"
#include "Null/Engine/Submodules/ECS/Components/BoxCollider2DComponent.h"

#include "Null/Engine/NEngine.h"
#include "Null/Engine/Misc/Time.h"
#include "Null/Engine/Misc/Input.h"
#include "Null/Tools/Trace.h"
#include "Null/Tools/JsonWrapper.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

using EntityID = uint32_t;


//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//
