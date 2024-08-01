
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
        vbo.Bind();

        //m_Instances += vbo.Instances();

        size_t offset = 0;
        for (const Layout& layout : vbo.Layouts())
        {
            if (layout.glType == GL_INT)
            {
                GL(glVertexAttribIPointer(m_LayoutCounter,
                    layout.count, layout.glType, vbo.Stride(), (void*)offset));
            }
            else
            {
                GL(glVertexAttribPointer(m_LayoutCounter,
                    layout.count, layout.glType, GL_FALSE, vbo.Stride(), (void*)offset));
            }

            offset += layout.size;
            GL(glEnableVertexAttribArray(m_LayoutCounter));

            if (vbo.Instances())
            {
                GL(glVertexAttribDivisor(m_LayoutCounter, 1));
            }

            m_LayoutCounter++;
        }
        Unbind();
        vbo.Unbind();
    }

    void VAO::UpdateVBO(const VBO& vbo)
    {
        m_Instances = vbo.Instances();
    }

    void VAO::AttachEBO(const EBO& ebo)
    {
        Bind();
        ebo.Bind();


        m_ElementCount = ebo.Count();
        m_DrawType = GL_TRIANGLES;

        Unbind();
        ebo.Unbind();
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
