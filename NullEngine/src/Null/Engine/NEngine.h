#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NEngine.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Null/Engine/Base/IEngine.h"
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


	class NLE_API NEngine : public IEngine
	{
	public:
		void Load() override;
		void Init() override;           // Initialize FLEngine
		void Update(float dt) override; // Update FLEngine
		void Unload() override;
		void Shutdown() override;       // Shutdown FLEngine

		void Add(IModule* component, const std::string_view& name) override;

		IModule* Get(const std::string_view& name) const override;

		//FrameBuffer* fbo = nullptr;

		static IEngine& Instance()

		{
			static NEngine instance;
			return instance;
		}


		static bool editor;

	private:
		int frames = 0;
		float framerateTimer = 0;

		NEngine() = default;
		//! Vector containing all FLEngine Components
		std::vector<std::pair<IModule*, std::string>> m_Modules;

	};


}