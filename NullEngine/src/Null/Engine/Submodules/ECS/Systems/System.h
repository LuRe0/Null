#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	System.h
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
	const unsigned int MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> SignatureBits;
	typedef std::vector<size_t> OwnedComponents;

	using EntityID = uint32_t;



	class NLE_API Signature {
	public:
		Signature() : m_Bitset(), m_Indices() {}

		void set(size_t index, bool value = true);

		void reset(size_t index);

		bool test(size_t index) const;

		size_t size();

		size_t count();
	

		const std::bitset<MAX_COMPONENTS>& GetBitset() const
		{
			return m_Bitset;
		}

		const OwnedComponents& GetSetIndices() const
		{
			return m_Indices;
		}



	private:
		std::bitset<MAX_COMPONENTS> m_Bitset;  // Adjust the size as needed
		OwnedComponents m_Indices;
	};

	class NLE_API NRegistry;

	class NLE_API ISystem : public IModule
	{
	public:
		virtual ~ISystem() {};

		virtual void Load()  override = 0;
		//! Virtual Init function
		virtual void Init() override;
		//! Virtual Update function
		virtual void Update(float dt) override = 0;
		virtual void RuntimeUpdate(float dt) override {}

		virtual void Render() override = 0;

		void RenderImGui() override;

		virtual void Unload()override = 0;
		//! Virtual Shutdown function
		virtual void Shutdown() override = 0;


		bool HasRenderImGui() const override { return !m_Entities.empty(); }


		/// <summary>
		/// Adds entity to system
		/// </summary>
		/// <param name="entity"></param>
		void Add(EntityID entity);


		/// <summary>
		/// removes an enitity from the system
		/// </summary>
		void Remove(EntityID entity);

		/// <summary>
		/// get all entities relevent to a system
		/// </summary>
		/// <returns></returns>
		const std::vector<EntityID>& GetSystemEntities() const;


		const SignatureBits& GetComponentSignature() const;

		/// <summary>
		/// Defines which components an entity must possess to be modified by system
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T> 
		void Require()
		{
			const auto id = Component<T>::GetID();
			m_ComponentSignatures.set(id);
		}

		//void SetParent(NRegistry* registry)
		//{
		//	m_Parent = registry;
		//}
	private:
		/// <summary>
		/// the componets an entity must possess in order to be modified by the system
		/// </summary>
		SignatureBits m_ComponentSignatures;

		/// <summary>
		/// entities that are modifiable by the system
		/// </summary>

		void CheckEntity(EntityID entityID, NRegistry* registry);
		void UpdateEntityList(EntityID entityID, NRegistry* registry);


		void OnEntityCreate(const EntityModifiedEvent& e);
		void OnEntityDestroyed(const EntityDestroyedEvent& e);
		void OnEntityComponentRemoved(const EntityRemoveComponentEvent& e);
		void OnSceneSwitch(const SceneSwitchEvent& e);


	protected:
		std::vector<EntityID> m_Entities;
		//NRegistry* m_Parent;
	};
	
	template <typename T>
	class System
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
