#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NShaderManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"



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
	class NLE_API Shader;

	class NLE_API NShaderManager : public IModule
	{
	public:
		NShaderManager() = default;

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void AddShader(const std::string& name, Shader* shader);

		Shader* GetShader(const std::string& name);

		Shader* CreateShader(const std::string& name);

	private:

		std::unordered_map < std::string, Shader* > mShaderList;

		NShaderManager(NShaderManager const&);
		NShaderManager& operator=(NShaderManager const&);
	};

}