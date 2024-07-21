  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VBO.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

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


		void AttachBuffer(const std::vector<float>& vertices, bool dynamic = false);

		void AttachBuffer(const std::vector<Vertex>& vertices, bool dynamic = false);

		const unsigned int GetID() const { return m_ID; }


	private:
		unsigned int m_ID;
		unsigned int m_Count = 0;
	};

}