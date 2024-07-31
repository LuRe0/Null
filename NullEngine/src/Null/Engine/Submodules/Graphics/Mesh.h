  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Mesh.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/
//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VAO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VBO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/EBO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BufferData.h"

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

	class NLE_API Mesh
	{
	public:


		Mesh(const std::string& filename);
		Mesh(const std::string& name, float xHalfSize, float yHalfSize, float uSize, float vSize);
		Mesh() = default;
		~Mesh();

		template <typename T>
		void SetupVertexBuffer(const std::vector<T>& vertexData) {
			m_Buffer.m_VBO.Bind();
			m_Buffer.m_VBO.AttachBuffer(vertexData);
		}
		void SetupIndexBuffer(const std::vector<unsigned int>& indexData);
		void SetupVertexAttributes();
		void Render(const SpriteSource* spriteSource) const;
		void RenderTexture(unsigned int texture) const;

		const std::string& GetName() { return m_Name; }
	private:
		struct Buffer
		{
			Buffer() = default;

			VAO m_VAO;
			VBO m_VBO;
			EBO m_EBO;
		};

		Buffer m_Buffer;

		float m_xHalfSize, m_yHalfSize;

		std::string m_Name;
	};



}