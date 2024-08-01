
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

/*
Code adapted from Cyrus Bozorgzad
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/



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

	unsigned int VBO::Stride() const
	{
		unsigned int counter = 0;
		for (const Layout& layout : m_Layouts)
		{
			counter += layout.size;
		}
		return counter;
	}


	void VBO::AttachBuffer(const std::vector<float>& vertices, std::vector<Layout>& layouts, bool dynamic, size_t size)
	{
		m_Layouts = layouts;
		m_Count = size;
		m_Instances = 0;

		dynamic ? glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW)
			: glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	}

	void VBO::AttachBuffer(const std::vector<Vertex>& vertices, std::vector<Layout>& layouts, bool dynamic, size_t size)
	{
		m_Layouts = layouts;
		m_Count = size;
		m_Instances = 0;
		dynamic ? glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW)
			: glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	}

	void VBO::AttachBuffer(const std::vector<Instance>& vertices, std::vector<Layout>& layouts, bool dynamic, size_t size)
	{
		m_Layouts = layouts;
		m_Count = size;
		dynamic ? glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(Instance), nullptr, GL_DYNAMIC_DRAW)
			: glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(Instance), nullptr, GL_STATIC_DRAW);
	}

	void VBO::UpdateBuffer(const std::vector<Instance>& vertices)
	{
		m_Instances = vertices.size();
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_Instances * sizeof(Instance), vertices.data());
	}
}
