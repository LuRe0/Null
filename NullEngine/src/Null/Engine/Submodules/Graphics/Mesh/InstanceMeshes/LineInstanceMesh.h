  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	LineInstanceMesh.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VAO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VBO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/EBO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BufferData.h"
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"


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

	class NLE_API LineInstanceMesh : public Mesh
	{
	public:


		LineInstanceMesh(const std::string& filename, const uint32_t vertexCount, const uint32_t indexCount);
		LineInstanceMesh() = default;
		~LineInstanceMesh();

		void Bind() const;
		void Unbind() const;


		template <typename T>
		void SetupInstanceBuffer(const std::vector<T>& vertexData, std::vector<Layout> layouts, bool dynamic = false, size_t size = 0) {
			m_Buffer.m_VBO.Bind();
			m_Buffer.m_VBO.AttachBuffer(vertexData, layouts, dynamic, size);
			m_Buffer.m_VBO.Unbind();
		}

		void SetupIndexBuffer(const std::vector<unsigned int>& indexData);

		void SetupVertexAttributes();
		void SetupInstances();

		template <typename T>
		void UpdateInstances(const std::vector<T>& vertexData, size_t size)
		{
	
			m_Buffer.m_VBO.Bind();

			m_Buffer.m_VBO.UpdateBuffer(vertexData);
			m_Buffer.m_VAO.UpdateVBO(m_Buffer.m_VBO);

			m_Buffer.m_VBO.Unbind();

		}

		void RenderLines(uint32_t count) const;
		void Render(uint32_t count) const;
		
	private:
	};
}