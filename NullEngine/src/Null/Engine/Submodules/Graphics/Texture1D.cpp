
//------------------------------------------------------------------------------
//
// File Name:	Texture1D.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Texture1D.h"
#include <glad/glad.h>  // only here in cpp



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void Texture1D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_1D, 0);
	}
	void Texture1D::Init(int size, unsigned int internalFormat, unsigned int format, unsigned int type)
	{
		m_Size = size;

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_1D, m_TextureID);

		// Allocate immutable storage (optional but preferred)
		glTexStorage1D(GL_TEXTURE_1D, 1, internalFormat, size);

		// Set filtering and wrap mode
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_1D, 0);
	}
	void Texture1D::Update(const int* data)
	{
		glBindTexture(GL_TEXTURE_1D, m_TextureID);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, m_Size, GL_RED_INTEGER, GL_INT, data);
		glBindTexture(GL_TEXTURE_1D, 0);
	}
	void Texture1D::BindUnit(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_1D, m_TextureID);
	}
	void Texture1D::Shutdown()
	{
		if (m_TextureID)
		{
			glDeleteTextures(1, &m_TextureID);
			m_TextureID = 0;
		}
	}
}
