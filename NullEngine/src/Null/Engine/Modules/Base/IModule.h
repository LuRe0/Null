#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	IModule.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Null/Core.h"
#include "Null/Engine/Base/IEngine.h"


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
	//! Base Module for systems
	class NLE_API IModule
	{
	public:
		IModule() :mParent(nullptr) {}

		virtual void Load() {};
		//! Virtual Init function
		virtual void Init() = 0;
		//! Virtual Update function
		virtual void Update(float dt) = 0;

		virtual void Unload() {};
		//! Virtual Shutdown function
		virtual void Shutdown() = 0;

		void SetParent(IEngine* engine) { mParent = engine; };

		IEngine* GetParent(void) const
		{
			return mParent;
		}

		template<typename T>
		T* Get() const
		{
			return GetParent()->Get<T>();
		}

		//! Virtual Deconstructor function
		virtual ~IModule() = default;

	private:
		IEngine* mParent;
	};
}