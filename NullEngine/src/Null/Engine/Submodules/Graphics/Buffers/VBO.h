  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VBO.h
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
#include "BufferData.h"



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
	//class Vertex;
	class VBO
	{
	public:
		VBO();

		
		//VBO(const std::vector<Vertex>& data);
		~VBO();

		void Bind() const;
		void Unbind() const;
		inline unsigned int Count() const { return m_Count; }
		inline const std::vector<Layout>& Layouts() const { return m_Layouts; }
		unsigned int Stride() const;
		unsigned int Instances() const { return m_Instances; }


		void AttachBuffer(const std::vector<float>& vertices, std::vector<Layout>& layouts,bool dynamic = false, size_t size = 0);

		void AttachBuffer(const std::vector<Vertex>& vertices, std::vector<Layout>& layouts, bool dynamic = false, size_t size = 0);

		void AttachBuffer(const std::vector<Instance>& vertices, std::vector<Layout>& layouts, bool dynamic = false, size_t size = 0);

		void UpdateBuffer(const std::vector<Instance>& vertices);

		void UpdateBuffer(const std::vector<Vertex>& vertices);

		const unsigned int GetID() const { return m_ID; }


	private:
		unsigned int m_ID;
		unsigned int m_Count = 0;
		std::vector<Layout> m_Layouts;
		unsigned int m_Instances = 0;
	};

}