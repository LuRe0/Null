
//------------------------------------------------------------------------------
//
// File Name:	UBO.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "UBO.h"
#include <glad/glad.h>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void UBO::GenerateBuffer()
	{
		if (m_ID != 0)
			DeleteBuffer();

		glGenBuffers(1, &m_ID);
	}

	void UBO::DeleteBuffer()
	{
		if (m_ID)
		{
			glDeleteBuffers(1, &m_ID);
			m_ID = 0;
			m_Capacity = 0;
		}
	}

	void UBO::Bind(unsigned int bindingPoint) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
	}

	void UBO::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UBO::Allocate(size_t size, unsigned int drawType)
	{
		m_Capacity = size;
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, drawType);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UBO::UpdateData(const void* data, size_t size, size_t offset) const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
