#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
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
	// View for a single entity

	template<typename Registry, typename... Components>
	class RegistryView {
	public:
		using ComponentTuple = std::tuple<EntityID, Components&...>;

		RegistryView(Registry* registry, EntityID id)
			: m_Registry(registry), m_Entity(id) {
			NLE_CORE_ASSERT(registry != nullptr, "Registry was nullptr");
		}

		ComponentTuple Get() {
			return { m_Entity, m_Registry->template GetComponent<Components>(m_Entity)... };
		}

	private:
		Registry* m_Registry;
		EntityID m_Entity;
	};


	// Packed view for many entities

	template<typename Registry, typename... Components>
	class RegistryPackedView {
	public:
		using ComponentTuple = std::tuple<EntityID, Components*...>;

		RegistryPackedView(Registry* registry)
			: m_Registry(registry) {
		}

		// Build with no filter
		RegistryPackedView& Build(const std::vector<EntityID>& entities) {
			m_Data.clear();
			for (EntityID id : entities) {
				if ((m_Registry->template HasComponent<Components>(id) && ...)) {
					m_Data.emplace_back(id, &m_Registry->template GetComponent<Components>(id)...);
				}
			}
			return *this;
		}

		// Build with filter that includes EntityID and component values
		template<typename Predicate>
		RegistryPackedView& BuildFiltered(const std::vector<EntityID>& entities, Predicate&& predicate) {
			m_Data.clear();
			for (EntityID id : entities) {
				if ((m_Registry->template HasComponent<Components>(id) && ...)) {
					ComponentTuple tuple(id, &m_Registry->template GetComponent<Components>(id)...);
					if (std::apply([&](EntityID eid, auto*... comps) {
						return predicate(eid, *comps...);
						}, tuple)) {
						m_Data.emplace_back(std::move(tuple));
					}
				}
			}
			return *this;
		}

		// Post-build filtering (optional; slowest)
		template<typename Predicate>
		void Filter(Predicate&& predicate) {
			std::vector<ComponentTuple> filtered;
			filtered.reserve(m_Data.size());

			for (auto& tuple : m_Data) {
				if (std::apply([&](EntityID eid, auto*... comps) {
					return predicate(eid, *comps...);
					}, tuple)) {
					filtered.push_back(tuple);
				}
			}

			m_Data = std::move(filtered);
		}

		const std::vector<ComponentTuple>& Data() const { return m_Data; }

	private:
		Registry* m_Registry;
		std::vector<ComponentTuple> m_Data;
	};


}

