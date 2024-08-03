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
#include <sol/sol.hpp>  // Include Sol header where sol::state is defined


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
		IModule() :m_Parent(nullptr) {}

		virtual void Load() {};
		//! Virtual Init function
		virtual void Init() = 0;
		//! Virtual Update function
		virtual void Update(float dt) = 0;

		virtual void Render() {};

		virtual void RenderImGuI() {};

		virtual void Unload() {};
		//! Virtual Shutdown function
		virtual void Shutdown() = 0;

		virtual void RegisterToScripAPI(sol::state& lua) = 0;

		void SetParent(IEngine* engine) { m_Parent = engine; };

		IEngine* GetParent(void) const
		{
			return m_Parent;
		}

		template<typename T>
		T* Get() const
		{
			return GetParent()->Get<T>();
		}


		virtual void RenderImGui() {}
		virtual bool HasRenderImGui() const { return false; }




		//! Virtual Deconstructor function
		virtual ~IModule() = default;

	protected:
		IEngine* m_Parent;
	};


	template <typename T>
	class NLE_API Module
	{
	public:
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:
	};
}