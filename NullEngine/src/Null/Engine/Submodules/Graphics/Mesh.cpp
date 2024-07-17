
//------------------------------------------------------------------------------
//
// File Name:	Mesh.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Mesh.h"
#include "glad/glad.h"
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Shader/Shader.h"
#include "Null/Engine/Submodules/Graphics/SpriteSource.h"



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



	Mesh::Mesh(float xHalfSize, float yHalfSize, float uSize, float vSize, const std::string& name) : m_xHalfSize(xHalfSize), m_yHalfSize(yHalfSize), m_uSize(uSize), m_vSize(vSize), m_Name(name)
	{
		// Define vertices for a triangle
		std::vector<Vertex> vertexData;

		vertexData.push_back({ glm::vec3(xHalfSize, yHalfSize, 0), glm::vec4(0.0f), glm::vec2(1.0f / vSize, 0) });
		vertexData.push_back({ glm::vec3(xHalfSize, -yHalfSize, 0), glm::vec4(0.0f), glm::vec2(1.0f / vSize, 1.0f / uSize) });
		vertexData.push_back({ glm::vec3(-xHalfSize, -yHalfSize, 0), glm::vec4(0.0f), glm::vec2(0, 1.0f / uSize) });
		vertexData.push_back({ glm::vec3(-xHalfSize, yHalfSize, 0), glm::vec4(0.0f), glm::vec2(0,0) });

		std::vector<float> v = {
			// positions                    // colors           // texture coords
			 xHalfSize,  yHalfSize, 0.0f,   1.0f, 0.0f, 0.0f,   uSize, vSize,   // top right
			 xHalfSize, -yHalfSize, 0.0f,   0.0f, 1.0f, 0.0f,   uSize, 0.0f,    // bottom right
			-xHalfSize, -yHalfSize, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,     // bottom left
			-xHalfSize,  yHalfSize, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, vSize     // top left 
		};

		std::vector<unsigned int> indexData = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
		};

		//m_Buffer.m_VAO.Bind();

		//m_Buffer.m_VBO.Bind();
		//m_Buffer.m_VBO.AttachBuffer(vertexData);
		//
		//m_Buffer.m_EBO.Bind();
		//m_Buffer.m_EBO.AttachBuffer(indexData);

		m_Buffer.m_VAO.Bind();

		SetupVertexBuffer(vertexData);
		SetupIndexBuffer(indexData);
		SetupVertexAttributes();

		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Unbind();
		m_Buffer.m_VAO.Unbind();

	}
	Mesh::~Mesh()
	{

	}

	void Mesh::SetupVertexBuffer(const std::vector<Vertex>& vertexData) {
		m_Buffer.m_VBO.Bind();
		m_Buffer.m_VBO.AttachBuffer(vertexData);
	}

	void Mesh::SetupIndexBuffer(const std::vector<unsigned int>& indexData) {
		m_Buffer.m_EBO.Bind();
		m_Buffer.m_EBO.AttachBuffer(indexData);
	}

	void Mesh::SetupVertexAttributes() {
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	}


	void Mesh::Render(const SpriteSource* spriteSource) const
	{

		if (spriteSource && spriteSource->GetTexture())
		{
			spriteSource->GetTexture()->Bind();
		}

		m_Buffer.m_VAO.Bind();
		m_Buffer.m_VBO.Bind();
		m_Buffer.m_EBO.Bind();

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Unbind();
		m_Buffer.m_VAO.Unbind();

		// Unbind the texture if it was bound
		if (spriteSource && spriteSource->GetTexture())
		{
			spriteSource->GetTexture()->Unbind();
		}

	}


}
