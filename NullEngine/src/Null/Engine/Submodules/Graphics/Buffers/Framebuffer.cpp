
//------------------------------------------------------------------------------
//
// File Name:	Framebuffer.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Framebuffer.h"
#include "glad/glad.h"
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VBO.h"
//#include <>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
#define GL(function) \
    do { \
        while (glGetError() != GL_NO_ERROR); \
        function; \
        GLenum glError = glGetError(); \
        if (glError != GL_NO_ERROR) { \
            NLE_CORE_ASSERT(false, "OpenGL Error [{0}]", glError); \
        } \
    } while(0)


	Framebuffer::Framebuffer(unsigned int width, unsigned int height) 
	{

		m_WinWidth = width;
		m_WinHeight = height;

		//glViewport(0, 0, width, height);

	}

	// Destructor
	Framebuffer::~Framebuffer()
	{
		//DeleteFramebuffer();
	}

	void Framebuffer::Init()
	{
		CreateFramebuffer(m_WinWidth, m_WinHeight);
	}

	void Framebuffer::Shutdown()
	{
		DeleteFramebuffer();
	}

	// Bind the Framebuffer
	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_WinWidth, m_WinHeight);
	}

	void Framebuffer::ClearRender() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		NRenderer::ClearRenderS();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int Framebuffer::GetColorAttachment(unsigned int index) const
	{
		NLE_CORE_ASSERT(m_ColorAttachments.size(), "Color Attachment index: {0} out of bounds", index);
		
		if (index >= m_ColorAttachments.size())
		{
			return m_ColorAttachments.at(0);
		}

		return m_ColorAttachments.at(index);
	}

	int Framebuffer::ReadPixels(unsigned int index, unsigned int x, unsigned int y) const
	{
		NLE_CORE_ASSERT(index < m_ColorAttachments.size(), "Color Attachment index: {0} out of bounds", index);
		const Format& format = m_Formats[index];

		glReadBuffer(GL_COLOR_ATTACHMENT0 + index); // Select the color attachment
		int pixelData = 0;
		glReadPixels(x, y, 1, 1, format.m_Format, format.m_Type, &pixelData); // Read pixel data

		return pixelData;
	}

	void Framebuffer::AddColorAttachment(const std::vector<Format>& formats)
	{
		m_Formats = formats;

		Bind();
		for (size_t i = 0; i < formats.size(); i++)
		{
			unsigned int colorAttachment;
			glGenTextures(1, &colorAttachment);
			glBindTexture(GL_TEXTURE_2D, colorAttachment);
			m_ColorAttachments.push_back(colorAttachment);
			glTexImage2D(GL_TEXTURE_2D, 0, formats[i].m_InternalFormat, m_WinWidth, m_WinHeight, 0, formats[i].m_Format, formats[i].m_Type, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachment, 0);

		}
		// Set the list of draw buffers

		if (m_ColorAttachments.size() > 1)
		{
			NLE_CORE_ASSERT((m_ColorAttachments.size() <= 4), "Exeeded max allowed Attachments");
			std::vector<GLenum> drawBuffers;
			for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
			{
				drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			}

			glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		}
		else
		{
			glDrawBuffer(GL_NONE);
		}
	



		Unbind();
	}

	void Framebuffer::SetColorAttachment(unsigned int index, unsigned int format, unsigned int attachmentType)
	{
		if (index >= m_ColorAttachments.size()) return;
		Bind();
		unsigned int colorAttachment = m_ColorAttachments[index];
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_WinWidth, m_WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, colorAttachment, 0);
		Unbind();
	}

	void Framebuffer::RemoveColorAttachment(unsigned int index)
	{
		if (index >= m_ColorAttachments.size()) return;
		Bind();
		glDeleteTextures(1, &m_ColorAttachments[index]);
		m_ColorAttachments.erase(m_ColorAttachments.begin() + index);
		Unbind();
	}


	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(unsigned int width, unsigned int height)
	{
		m_WinWidth = width;
		m_WinHeight = height;
		if (!m_ColorAttachments.size()) return;

		Bind();
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);

		for (size_t i = 0; i < m_Formats.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, m_Formats[i].m_InternalFormat, width, height, 0, 
				m_Formats[i].m_Format, m_Formats[i].m_Type, NULL);
			
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		Unbind();
	}

	void Framebuffer::ClearColorAttachment(unsigned int index, const void* value) const
	{
		NLE_CORE_ASSERT(index < m_ColorAttachments.size(), "Color Attachment index: {0} out of bounds", index);

		// Clear the texture image
		const Format& format = m_Formats[index];

		glClearTexImage(m_ColorAttachments[index], 0, format.m_Format, format.m_Type, value);
	}

	void Framebuffer::CreateFramebuffer(unsigned int width, unsigned int height)
	{
		if (height == 0 || width == 0) return;

		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		// Create depth buffer
		glGenRenderbuffers(1, &m_RenderbufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

		// Unbind the framebuffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::DeleteFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		for (auto attachment : m_ColorAttachments)
		{
			glDeleteTextures(1, &attachment);
		}
		m_ColorAttachments.clear();
		glDeleteRenderbuffers(1, &m_RenderbufferID);
	}

}
