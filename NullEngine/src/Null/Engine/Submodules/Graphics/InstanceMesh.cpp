
//------------------------------------------------------------------------------
//
// File Name:	InstanceMesh.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "InstanceMesh.h"
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



	InstanceMesh::InstanceMesh(const std::string& filename,const uint32_t vertexCount, const uint32_t indexCount) : Mesh()
	{
		Read(filename);

		std::vector<Layout> instancesLayouts;

		instancesLayouts.push_back({ 3, GL_FLOAT, 3 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });

		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });

		// glm::vec4 color
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });

		// glm::vec2 texCoords
		instancesLayouts.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });

		// glm::vec2 texSize
		instancesLayouts.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });

		// unsigned int texIndex
		instancesLayouts.push_back({ 1, GL_UNSIGNED_INT, sizeof(unsigned int) });

		// uint32_t entityID
		instancesLayouts.push_back({ 1, GL_UNSIGNED_INT, sizeof(uint32_t) });

		SetupVertexBuffer(std::vector<Instance>(), instancesLayouts, true, vertexCount);


		m_Buffer.m_VAO.AttachVBO(m_Buffer.m_VBO);

		//to do
		std::vector<unsigned int> indexData(indexCount);
		uint32_t offset = 0;
		for (size_t i = 0; i < indexData.size(); i += 6)
		{
			indexData[i + 0] = offset + 0;
			indexData[i + 1] = offset + 1;
			indexData[i + 2] = offset + 2;

			indexData[i + 3] = offset + 2;
			indexData[i + 4] = offset + 3;
			indexData[i + 5] = offset + 0;

			offset += 4;
		}

		SetupIndexBuffer(indexData);

		m_Buffer.m_VAO.AttachEBO(m_Buffer.m_EBO);
	}
	InstanceMesh::~InstanceMesh()
	{

	}

	void InstanceMesh::Bind() const
	{
		m_Buffer.m_VAO.Bind();
		m_Buffer.m_VBO.Bind();
		m_Buffer.m_EBO.Bind();
	}

	void InstanceMesh::Unbind() const
	{
		m_Buffer.m_EBO.Unbind();
		m_Buffer.m_VBO.Bind();
		m_Buffer.m_VAO.Unbind();
	}

	void InstanceMesh::SetupIndexBuffer(const std::vector<unsigned int>& indexData) {
		m_Buffer.m_EBO.Bind();
		m_Buffer.m_EBO.AttachBuffer(indexData);
		m_Buffer.m_EBO.Unbind();
	}

	void InstanceMesh::SetupVertexAttributes() 
	{
		// Vertex struct attributes
		// Vertex Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Vertex Color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		// Vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textCoords));


	

		//m_Buffer.m_VAO.AttachVBO()
	}

	void InstanceMesh::SetupInstances()
	{
	/*	m_Buffer.m_InstanceVBO.Bind();*/
		std::vector<Layout> instancesLayouts;

		// glm::mat4 position (4 x vec4)
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });

		// glm::vec4 color
		instancesLayouts.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });

		// glm::vec2 texCoords
		instancesLayouts.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });

		// glm::vec2 texSize
		instancesLayouts.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });

		// unsigned int texIndex
		instancesLayouts.push_back({ 1, GL_UNSIGNED_INT, sizeof(unsigned int) });

		// uint32_t entityID
		instancesLayouts.push_back({ 1, GL_UNSIGNED_INT, sizeof(uint32_t) });

		//SetupInstanceBuffer(std::vector<Instance>(), instancesLayouts, true, MAXVERTEXCOUNT);
	}


	void InstanceMesh::Render(uint32_t count) const
	{
		NTextureManager* textureMan = NEngine::Instance().Get<NTextureManager>();
		
		const auto& textureNames = textureMan->GetResourceNames();

	

		m_Buffer.m_VAO.Bind();

		//for (size_t i = 0; i < textureNames.size(); i++)
		//{
		//	Texture* texture = textureMan->Get(textureNames[i]);
		//	uint32_t index = textureMan->GetTextureIndex(textureNames[i]);
		//	glBindTextureUnit(index, texture->GetID());
		//}

		glDrawElements(m_Buffer.m_VAO.DrawType(), count, GL_UNSIGNED_INT, nullptr);


		m_Buffer.m_VAO.Unbind();

	}


}
