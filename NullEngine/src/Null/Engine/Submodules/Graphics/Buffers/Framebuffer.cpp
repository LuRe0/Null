
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
	}

	void Framebuffer::ClearRender() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		NRenderer::ClearRenderS();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(unsigned int width, unsigned int height)
	{
		//DeleteFramebuffer();
		//CreateFramebuffer(width, height);
		Bind();
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Unbind();

		//// Update viewport
		//glViewport(0, 0, width, height);
	}

	//void Framebuffer::SaveState()
	//{
	//    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	//    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m);
	//    ////tex.SetDimensions(windowSize);
	//    //glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

	void Framebuffer::CreateFramebuffer(unsigned int width, unsigned int height)
	{
		if (height == 0 || width == 0) return;

		//// Create Framebuffer
		//glCreateFramebuffers(1, &m_FramebufferID);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		//// Create Color Attachment Texture
		//glGenTextures(1, &m_ColorAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glBindTexture(GL_TEXTURE_2D, 0);

		//// attach it to currently bound framebuffer object
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		////glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		////glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		////glTextureStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
		////glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		//// Create Renderbuffer Object for Depth and Stencil Attachment
		//glGenRenderbuffers(1, &m_RenderbufferID);
		//glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//// Check if Framebuffer is complete
		//NLE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");

		



		//glBindFramebuffer(GL_FRAMEBUFFER, 0);



		glGenFramebuffers(1, &m_FramebufferID);
		glGenRenderbuffers(1, &m_RenderbufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);



		glGenTextures(1, &m_ColorAttachment);

		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
		auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		NLE_CORE_ASSERT(fboStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");

		glBindTexture(GL_TEXTURE_2D, 0);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::DeleteFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteRenderbuffers(1, &m_RenderbufferID);
	}

}
