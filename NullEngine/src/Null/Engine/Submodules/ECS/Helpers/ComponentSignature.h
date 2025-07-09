#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
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
	const unsigned int MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> SignatureBits;
	typedef std::vector<size_t> OwnedComponents;

	using EntityID = uint32_t;



	class NLE_API Signature {
	public:
		Signature() : m_Bitset(), m_Indices() {}

		void set(size_t index, bool value = true);

		void reset(size_t index);

		bool test(size_t index) const;

		size_t size();

		size_t count();


		const std::bitset<MAX_COMPONENTS>& GetBitset() const
		{
			return m_Bitset;
		}

		const OwnedComponents& GetSetIndices() const
		{
			return m_Indices;
		}



	private:
		std::bitset<MAX_COMPONENTS> m_Bitset;  // Adjust the size as needed
		OwnedComponents m_Indices;
	};

}