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
	class NLE_API IComponent
	{
	protected:
		static uint32_t m_NextID;
	};

	template <typename T>
	class NLE_API Component : public IComponent
	{
	public:
	
		static uint32_t GetID()
		{
			static uint32_t id = ++m_NextID;
			return id;
		}

		
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:
	};

	struct BaseComponent 
	{
		virtual ~BaseComponent() = default;
		virtual const std::string Name() const = 0;
		virtual const uint32_t ID() const = 0;

		BaseComponent() = default;

		bool m_Enabled = true;
		bool m_SerializeToScene = true;
	};

}