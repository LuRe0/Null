
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

using JSON = nlohmann::json;


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



	Mesh::Mesh(const std::string& filename) : m_xHalfSize(.5f), m_yHalfSize(.5f), m_Name("")
	{
		// Define vertices for a triangle
		std::vector<Vertex> vertexData;


		std::vector<unsigned int> indexData;

		std::string filePath =std::string("../Data/Meshes/") + filename + std::string(".json");

		std::ifstream file(filePath);

		NLE_CORE_ASSERT(file.is_open(), "Could not open file ", filePath);

		JSON j;
		file >> j;

		m_xHalfSize = j["xHalfSize"];
		m_yHalfSize = j["yHalfSize"];
		m_Name = filename;

		for (const auto& vertex : j["vertices"]) {
			Vertex v;
			v.position = glm::vec3(vertex["position"][0], vertex["position"][1], vertex["position"][2]);
			v.color = glm::vec4(vertex["color"][0], vertex["color"][1], vertex["color"][2], vertex["color"][3]);
			v.textCoords = glm::vec2(vertex["texCoords"][0], vertex["texCoords"][1]);
			vertexData.push_back(v);
		}


		for (const auto& index : j["indices"]) {
			indexData.push_back(index);
		}

		m_Buffer.m_VAO.Bind();

		SetupVertexBuffer(vertexData);
		SetupIndexBuffer(indexData);
		SetupVertexAttributes();

		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Unbind();
		m_Buffer.m_VAO.Unbind();

	}
	Mesh::Mesh(const std::string& name, float xHalfSize, float yHalfSize, float uSize, float vSize) :
		m_xHalfSize(xHalfSize), m_yHalfSize(yHalfSize), m_Name(name)
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
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textCoords));
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

		glDrawElements(GL_TRIANGLES, m_Buffer.m_EBO.GetSize(), GL_UNSIGNED_INT, 0);

		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Unbind();
		m_Buffer.m_VAO.Unbind();

		// Unbind the texture if it was bound
		if (spriteSource && spriteSource->GetTexture())
		{
			spriteSource->GetTexture()->Unbind();
		}

	}

	void Mesh::RenderTexture(unsigned int texture) const
	{

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindSampler(1, 1);


		m_Buffer.m_VAO.Bind();
		m_Buffer.m_VBO.Bind();
		m_Buffer.m_EBO.Bind();

		glDrawElements(GL_TRIANGLES, m_Buffer.m_EBO.GetSize(), GL_UNSIGNED_INT, 0);

		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Unbind();
		m_Buffer.m_VAO.Unbind();

		glBindTexture(GL_TEXTURE_2D, 0);
	}


}
