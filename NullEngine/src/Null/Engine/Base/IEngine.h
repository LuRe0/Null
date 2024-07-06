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
			return dynamic_cast<T*>(Get(T::Name()));
		}

		template<typename T, typename ... Args>
		void Add(Args&&... args)
		{
			Add(new T(std::forward<Args>(args)...), T::Name());
		}

		virtual ~IEngine() = default;
	};


}