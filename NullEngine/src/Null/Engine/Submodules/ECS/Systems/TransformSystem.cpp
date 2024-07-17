
//------------------------------------------------------------------------------
//
// File Name:	TransformSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "TransformSystem.h"
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
	void TransformSystem::Load()
	{
	}

	void TransformSystem::Init()
	{
		//NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &TransformSystem::OnWindowResize, eventManager);
	}

	void TransformSystem::Update(float dt)
	{
		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);

			if (transform.m_Dirty) 
			{
				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.m_Translation);
				// Calculate rotation matrix (assuming Euler angles in radians)
				glm::mat4 rotationMatrix = glm::mat4(1.0f);

				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.m_Scale);

				transform.m_TransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

				transform.m_Dirty = false;
			}
		}
	}


	void TransformSystem::Render() const
	{
	}

	void TransformSystem::Unload()
	{
	}

	void TransformSystem::Shutdown()
	{
	}

}