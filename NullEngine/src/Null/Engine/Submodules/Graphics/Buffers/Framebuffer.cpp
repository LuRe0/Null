
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
        // Create Framebuffer
        glGenFramebuffers(1, &m_FramebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

        // Create Color Attachment Texture
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        // Create Renderbuffer Object for Depth and Stencil Attachment
        glGenRenderbuffers(1, &m_RenderbufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

        // Check if Framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            NLE_CORE_ERROR("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Destructor
    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_FramebufferID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteRenderbuffers(1, &m_RenderbufferID);
    }

    // Bind the Framebuffer
    void Framebuffer::Bind() const 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
