#pragma once


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VBO.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/

#include "stdafx.h"
#include "Null/Core.h"
#include "SSBO.h"
#include "Null/Engine/Submodules/ECS/Components/ParticleSystemComponent.h"
#include <glad/glad.h>

namespace NULLENGINE
{
	void SSBO::GenerateBuffer()
	{
		glGenBuffers(1, &m_ID);
	}

	void SSBO::DeleteBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void SSBO::Bind(GLuint bindingPoint) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ID);
	}

	void SSBO::Unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void SSBO::AllocateParticleBuffer(size_t count)
	{
		m_Capacity = count;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(ParticleInstance), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

}
