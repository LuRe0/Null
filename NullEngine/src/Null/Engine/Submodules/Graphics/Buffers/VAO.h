  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VAO.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

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

		void AttachEBO(const VBO& vbo);

	private:
		unsigned int m_ID;

	};


}