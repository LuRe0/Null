  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EBO.h
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

	class EBO
	{
	public:
		void AttachBuffer(std::vector<unsigned int> data, bool dynamic = false);

		EBO();

		~EBO();

		void Bind() const;
		void Unbind() const;

		const unsigned int GetID() const { return m_ID; }
		inline unsigned int Count() const { return m_Count; }
		inline unsigned int Type() const { return m_Type; }
	private:
		unsigned int m_ID;
		unsigned int m_Count = 0;
		unsigned int m_Type;
	};


}