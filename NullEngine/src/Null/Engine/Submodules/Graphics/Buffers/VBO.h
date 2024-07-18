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

		template <typename T>
		void AttachBuffer(const std::vector<T>& vertices, bool dynamic = false)
		{
			m_Count = vertices.size();
			dynamic ? glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_DYNAMIC_DRAW)
				: glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);
		}

		const unsigned int GetID() const { return m_ID; }


	private:
		unsigned int m_ID;
		unsigned int m_Count = 0;
	};


}