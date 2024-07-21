  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	InstanceMesh.h
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
#include "Null/Engine/Submodules/Graphics/Mesh.h"

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

	class NLE_API InstanceMesh : public Mesh
	{
	public:


		InstanceMesh(const std::string& filename);
		~InstanceMesh();

		template <typename T>
		void SetupVertexBuffer(const std::vector<T>& vertexData) {
			m_Buffer.m_VBO.Bind();
			m_Buffer.m_VBO.AttachBuffer(vertexData);
		}
		void SetupIndexBuffer(const std::vector<unsigned int>& indexData);
		void SetupVertexAttributes();
		void Render(const SpriteSource* spriteSource) const;

	private:
		struct Buffer
		{
			Buffer() = default;

			VAO m_VAO;
			VBO m_VBO;
			EBO m_EBO;
		};

		Buffer m_Buffer;

		float m_xHalfSize, m_yHalfSize, m_uSize, m_vSize;

		std::string m_Name;
	};



}