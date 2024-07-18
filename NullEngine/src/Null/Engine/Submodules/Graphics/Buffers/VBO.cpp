
//------------------------------------------------------------------------------
//
// File Name:	VBO.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "VBO.h"
#include "glad/glad.h"
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"



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



    //VBO(const std::vector<float>& vertices) 
    //{

    //}

    VBO::VBO()
    {
        glGenBuffers(1, &m_ID);
    }

    //VBO::VBO(const std::vector<float>& vertices)
    //{

    //  glBindBuffer(GL_ARRAY_BUFFER, m_ID);


    //  glBindBuffer(GL_ARRAY_BUFFER, 0);
    //}



    VBO::~VBO()
    {
        glDeleteBuffers(1, &m_ID);
    }
    void VBO::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }
    void VBO::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    unsigned int VBO::Count() const
    {
        return m_Count;
    }


}
