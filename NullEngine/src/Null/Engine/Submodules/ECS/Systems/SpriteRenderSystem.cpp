
//------------------------------------------------------------------------------
//
// File Name:	SpriteRenderSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "SpriteRenderSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void SpriteRenderSystem::Load()
	{
	}

	void SpriteRenderSystem::Init()
	{
	}

	void SpriteRenderSystem::Update(float dt)
	{

	}

	void SpriteRenderSystem::Render() const
	{
		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);

			//model, mesh, spritesrc, tint, shadername, frameindex
			renderer->AddRenderCall({ transform.m_TransformMatrix, sprite.m_Mesh,sprite.m_SpriteSource, sprite.m_Color, sprite.m_ShaderName, sprite.m_FrameIndex });
		}


	}

	void SpriteRenderSystem::Unload()
	{
	}

	void SpriteRenderSystem::Shutdown()
	{
	}
}