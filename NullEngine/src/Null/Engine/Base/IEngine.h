#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	IEngine.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
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

	class NLE_API IModule;

	struct NLE_API IEngine
	{
		virtual void Add(IModule* module, const std::string_view& name) = 0;
		virtual IModule* Get(const std::string_view& name) const = 0;

		virtual void Load() {};
		virtual void Init() = 0;
		virtual void Update(float dt) = 0;
		virtual void Unload() {};
		virtual void Shutdown() = 0;
		template<typename T>
		T* Get() const
		{
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
	
	
			return dynamic_cast<T*>(Get(fullName.substr(pos + 1)));
		}

		template<typename T, typename ... Args>
		void Add(Args&&... args)
		{
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');


			Add(new T(std::forward<Args>(args)...), fullName.substr(pos + 1));
		}

		virtual ~IEngine() = default;
	};


}