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

	template <typename T>
	class NLE_API NResourceManager : public IModule
	{
	public:
		NResourceManager() = default;

		virtual void Load() override {};

		//! Virtual Init function
		virtual void Init() override {};

		//! Virtual Update function
		virtual void Update(float dt) override {};

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
			NLE_CORE_ASSERT(!m_ResourceList.contains(name), "resource {0} already exists", name);


			m_ResourceList[name] = resource;
		}

		T* Get(const std::string& name)
		{
			NLE_CORE_ASSERT(m_ResourceList.contains(name), "resource {0} does not exist", name);
			return m_ResourceList.at(name);
		}

		bool Has(const std::string& name) const
		{
			return m_ResourceList.contains(name);
		}

		template <typename ...TArgs>
		T* Create(const std::string& name, TArgs&& ...args)
		{
			if (!m_ResourceList.contains(name))
			{
				T* resource = new T(name,std::forward<TArgs>(args)...);

				Add(name, resource);

				return resource;
			}


			return m_ResourceList.at(name);
		}

		std::vector<std::string> GetResourceNames() const
		{
			std::vector<std::string> componentNames;
			for (const auto& pair : m_ResourceList) {
				componentNames.push_back(pair.first);
			}
			return componentNames;
		}

	private:

		std::unordered_map<std::string, T*> m_ResourceList;

		NResourceManager(NResourceManager const&);
		NResourceManager& operator=(NResourceManager const&);
	};





}