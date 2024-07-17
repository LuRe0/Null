
//------------------------------------------------------------------------------
//
// File Name:	VAO.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "VAO.h"
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



    VAO::VAO() 
    {
        glGenVertexArrays(1, &m_ID);
    }

    // Destructor
    VAO::~VAO() {
        glDeleteVertexArrays(1, &m_ID);
    }

    // Bind the VAO
    void VAO::Bind() const {
        glBindVertexArray(m_ID);
    }

    // Unbind the VAO
    void VAO::Unbind() const {
        glBindVertexArray(0);
    }

    void VAO::AttachVBO(const VBO& vbo)
    {
        Bind();
    }

    void VAO::AttachEBO(const VBO& vbo)
    {
    }

    //void VAO::Attach() const
    //{
    //    //Bind();
    // /*   glBindBuffer(GL_ARRAY_BUFFER, vbo.GetID());*/
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //    glEnableVertexAttribArray(0);

    //    // color attribute
    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //    glEnableVertexAttribArray(1);

    //    // texture coord attribute
    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //    glEnableVertexAttribArray(2);
    //    //Unbind();

    //}

}
