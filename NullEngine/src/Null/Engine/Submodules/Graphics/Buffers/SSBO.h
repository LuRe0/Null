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

#include "Null/Core.h"

namespace NULLENGINE
{
	class SSBO
	{
	public:
		SSBO() { }
		~SSBO() { }

		void GenerateBuffer();

		void DeleteBuffer();

		void Bind(unsigned int bindingPoint = 0) const;

		void Unbind() const;

		void AllocateParticleBuffer(size_t count);

		unsigned int GetID() const { return m_ID; }
		size_t Capacity() const { return m_Capacity; }

	private:
		unsigned int m_ID = 0;
		size_t m_Capacity = 0;
	};
}
