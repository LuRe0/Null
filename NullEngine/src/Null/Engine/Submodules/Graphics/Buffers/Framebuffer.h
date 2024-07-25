  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Framebuffer.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{

	class VBO;

	class Framebuffer
	{
	public:
        Framebuffer(unsigned int width, unsigned int height);
        //Framebuffer() = default;
        ~Framebuffer();

        void Init();
        void Shutdown();

        void Bind() const;
        void Unbind() const;

        void Resize(unsigned int width, unsigned int height);

        void ClearRender() const;

        unsigned int GetColorAttachment() const { return m_ColorAttachment; }

    private:
        void CreateFramebuffer(unsigned int width, unsigned int height);
        void DeleteFramebuffer();

        unsigned int m_FramebufferID;
        unsigned int m_ColorAttachment;
        unsigned int m_DepthAttachment;
        unsigned int m_RenderbufferID;

        float m_WinWidth;
        float m_WinHeight;
	};


}