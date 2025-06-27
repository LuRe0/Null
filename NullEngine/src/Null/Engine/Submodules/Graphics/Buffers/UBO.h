#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	UBO.h
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
	class NLE_API UBO
	{
	public:
		UBO() { GenerateBuffer(); }
		~UBO() { DeleteBuffer(); }

		void GenerateBuffer();
		void DeleteBuffer();

		void Bind(unsigned int bindingPoint) const;
		void Unbind() const;

		void Allocate(size_t size, unsigned int drawType);
		void UpdateData(const void* data, size_t size, size_t offset = 0) const;

		unsigned int GetID() const { return m_ID; }
		size_t Capacity() const { return m_Capacity; }

	private:
		unsigned int m_ID = 0;
		size_t m_Capacity = 0;

	};

}