#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NTextureManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Graphics/Texture.h"


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
	// Specialize for resource
	class NLE_API NTextureManager : public NResourceManager<Texture>
	{
	public:


		void Load() override;

		//! Virtual Init function
		void Init() override {};

		//! Virtual Update function
		void Update(float dt) override {};

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override {};

		template <typename ...TArgs>
		Texture* Create(const std::string& name, TArgs&& ...args)
		{
			if (!Has(name))
			{
				Texture* resource = new Texture(name, std::forward<TArgs>(args)...);

				AddTextureIndex(name);

				Add(name, resource);

				return resource;
			}


			return Get(name);
		}


		void AddTextureIndex(const std::string& texture);

		uint32_t GetTextureIndex(const std::string& texture);

		const std::unordered_map<std::string, uint32_t>& GetTextureSlots() { return m_TextureSlots; }

	private:
		uint32_t CreateIndex() 
		{
			return m_NextIndex++;
		}
		
		std::unordered_map<std::string, uint32_t> m_TextureSlots;
		uint32_t m_NextIndex;
	};

}