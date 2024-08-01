  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VAO.h
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

	class VBO;
	class EBO;

	class VAO
	{
	public:
		VAO();
		~VAO();

		void Bind() const;
		void Unbind() const;

		const unsigned int GetID() const { return m_ID; }

		//void Attach() const;

		void AttachVBO(const VBO& vbo);

		void UpdateVBO(const VBO& vbo);

		void AttachEBO(const EBO& ebo);

		unsigned int ElementCount() const { return m_ElementCount; };
		unsigned int DrawType() const { return m_DrawType; }
		unsigned int Instances() const { return m_Instances; }


	private:
		unsigned int m_ID;

		unsigned int m_LayoutCounter = 0;

		unsigned m_ElementCount = 0;

		unsigned int m_DrawType;

		unsigned int m_Instances = 0;
	};


}