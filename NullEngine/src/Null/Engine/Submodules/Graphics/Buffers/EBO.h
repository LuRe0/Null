  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EBO.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "glad/glad.h"


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
		const unsigned int GetSize() const { return m_Size; }


	private:
		unsigned int m_ID;
		unsigned int m_Size;
	};


}