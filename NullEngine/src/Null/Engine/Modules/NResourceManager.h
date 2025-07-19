#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NResourceManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "../Modules/NStringIDManager.h"



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

	template<typename T, typename Derived>
	class NResourceManager : public ModuleBase<Derived>
	{
	public:
		NResourceManager() = default;

		virtual void Load() override {};

		//! Virtual Init function
		virtual void Init() override {};

		//! Virtual Update function
		virtual void Update(float dt) override {};
		virtual void RuntimeUpdate(float dt) override {};

		virtual void Unload() override
		{
			for (auto& resource : m_ResourceList)
			{
				delete resource.second;
			}

			m_ResourceList.clear();
		}
		//! Virtual Shutdown function
		virtual void Shutdown() override {}

		void Add(const std::string& name, T* resource)
		{
			NLE_CORE_ASSERT(!m_ResourceList.contains(STRID(name)), "resource {0} already exists", name);


			m_ResourceList[STRID(name)] = resource;
		}

		T* Get(const std::string& name)
		{
			if (!m_ResourceList.contains(STRID(name)))
			{
				NLE_CORE_WARN("resource {0} does not exist", name);
				return nullptr;
			}

			return m_ResourceList.at(STRID(name));
		}


		bool Has(const std::string& name) const
		{
			return m_ResourceList.contains(STRID(name));
		}


		T* Get(const uint32_t nameID)
		{

			if (!m_ResourceList.contains(nameID))
			{
				NLE_CORE_WARN("resource {0} does not exist", STRFROM(nameID));
				return nullptr;
			}
	
			return m_ResourceList.at(nameID);
		}

		bool Has(const uint32_t nameID) const
		{
			return m_ResourceList.contains(nameID);
		}

		template <typename ...TArgs>
		T* Create(const std::string& name, TArgs&& ...args)
		{
			if (!m_ResourceList.contains(STRID(name)))
			{
				T* resource = new T(name, std::forward<TArgs>(args)...);

				Add(name, resource);

				return resource;
			}


			return m_ResourceList.at(STRID(name));
		}

		std::vector<std::string> GetResourceNames() const
		{
			std::vector<std::string> componentNames;
			componentNames.reserve(m_ResourceList.size());
			for (const auto& pair : m_ResourceList) {
				componentNames.push_back(STRFROM(pair.first));
			}
			return componentNames;
		}

		void RegisterToScripAPI(sol::state& lua) override {};
	protected:
		std::unordered_map<uint32_t, T*> m_ResourceList;
	private:


		NResourceManager(NResourceManager const&);
		NResourceManager& operator=(NResourceManager const&);
	};





}