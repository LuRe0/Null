
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

    VBO::VBO()
    {
        glGenBuffers(1, &m_ID);
    }

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

    void VBO::AttachBuffer(const std::vector<float>& vertices, bool dynamic)
    {
        m_Count = vertices.size();
        dynamic ? glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW)
            : glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }

    void VBO::AttachBuffer(const std::vector<Vertex>& vertices, bool dynamic)
    {
        m_Count = vertices.size();
        dynamic ? glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW)
            : glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    }
}
