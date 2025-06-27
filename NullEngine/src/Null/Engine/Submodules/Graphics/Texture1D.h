#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Texture1D.h
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
	class NLE_API Texture1D
	{
	public:
		Texture1D() = default;
		~Texture1D() { Shutdown(); }

		void Init(int size, unsigned int internalFormat = 0x8235 /*GL_R32I*/,
			unsigned int format = 0x8D94 /*GL_RED_INTEGER*/,
			unsigned int type = 0x1404 /*GL_INT*/);

		//void Init(int size, GLenum internalFormat = GL_R32I, GLenum format = GL_RED_INTEGER, GLenum type = GL_INT);



		void Update(const int* data);


		void BindUnit(uint32_t slot) const;

		void Unbind() const;
	

		void Shutdown();

		unsigned int GetID() const { return m_TextureID; }
		int GetSize() const { return m_Size; }

	private:
		unsigned int m_TextureID = 0;
		int m_Size = 0;
	};
}