
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



	Mesh::Mesh(const std::string& filename) : m_xHalfSize(.5f), m_yHalfSize(.5f), m_uSize(1), m_vSize(1), m_Name("")
	{
		// Define vertices for a triangle
		std::vector<Vertex> vertexData;


		std::vector<unsigned int> indexData;

		std::string filePath =std::string("Data/Meshes/") + filename + std::string(".json");

		std::ifstream file(filePath);

		NLE_CORE_ASSERT(file.is_open(), "Could not open file ", filePath);

		JSON j;
		file >> j;

		m_xHalfSize = j["xHalfSize"];
		m_yHalfSize = j["yHalfSize"];
		m_uSize = j["uSize"];
		m_vSize = j["vSize"];
		m_Name = j["name"];

		for (const auto& vertex : j["vertices"]) {
			Vertex v;
			v.position = glm::vec3(vertex["position"][0], vertex["position"][1], vertex["position"][2]);
			v.color = glm::vec4(vertex["color"][0], vertex["color"][1], vertex["color"][2], vertex["color"][3]);
			v.textCoords = glm::vec2(vertex["texCoords"][0], vertex["texCoords"][1]);
			vertexData.push_back(v);
		}

		// Update UV coordinates according to uSize and vSize
		for (auto& vertex : vertexData) {
			vertex.textCoords.x /= m_uSize;
			vertex.textCoords.y /= m_vSize;
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


}
