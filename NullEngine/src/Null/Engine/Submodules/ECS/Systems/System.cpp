
//------------------------------------------------------------------------------
//
// File Name:	ISystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "System.h"
#include "Null/Tools/Trace.h"
#include "Null/Engine/Submodules/Events/IEvents.h"
#include "imgui.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	


	void Signature::set(size_t index, bool value)
	{
		assert(index < m_Bitset.size());

		if (value == m_Bitset.test(index)) return;

		m_Bitset.set(index, value);

		if (m_Bitset.test(index))
		{
			m_Indices.push_back(index);
		}
		else
		{
			m_Indices.erase(std::remove(m_Indices.begin(), m_Indices.end(), index), m_Indices.end());
		}
	}

	void Signature::reset(size_t index)
	{
		assert(index < m_Bitset.size());
		if (m_Bitset.test(index)) 
		{
			m_Bitset.reset(index);
			m_Indices.erase(std::remove(m_Indices.begin(), m_Indices.end(), index), m_Indices.end());
		}
	}

	bool Signature::test(size_t index) const
	{
		return m_Bitset.test(index);
	}

	size_t Signature::size()
	{
		return m_Bitset.size();
	}

	size_t Signature::count()
	{
		return m_Indices.size();
	}

}
