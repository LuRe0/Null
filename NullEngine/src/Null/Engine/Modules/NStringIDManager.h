#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NWindow.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include <limits>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
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
	static constexpr uint32_t InvalidID = std::numeric_limits<uint32_t>::max();



	class NLE_API NStringIDManager : public ModuleBase<NStringIDManager>
	{
	public:

		NStringIDManager();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;
		//void Render() override;
		void RenderLoadScreen();
		void Unload() override;
		void Shutdown() override;

		uint32_t GetOrCreateID(const std::string& str);

		std::optional<uint32_t> TryGetID(const std::string& str) const;

		const std::string& GetString(uint32_t id) const;

		bool HasRenderImGui() const override { return true; }

		void RegisterToScripAPI(sol::state& lua) override;

		void RenderImGui() override;

	private:
		std::unordered_map<std::string, uint32_t> m_StringToID;
		std::vector<std::string> m_IDToString;

		uint32_t m_NextID = 0;
	};


#define STRID(str)      NULLENGINE::NStringIDManager::Instance()->GetOrCreateID(str)
#define STRFROM(id)     NULLENGINE::NStringIDManager::Instance()->GetString(id)

}