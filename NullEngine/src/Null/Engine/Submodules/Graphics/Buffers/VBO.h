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
		unsigned int Count() const;


		void AttachBuffer(const std::vector<float>& vertices, bool dynamic = false, size_t size = 0);

		void AttachBuffer(const std::vector<Vertex>& vertices, bool dynamic = false, size_t size = 0);

		void AttachBuffer(const std::vector<Instance>& vertices, bool dynamic = false, size_t size = 0);

		const unsigned int GetID() const { return m_ID; }


	private:
		unsigned int m_ID;
		unsigned int m_Count = 0;
	};

}