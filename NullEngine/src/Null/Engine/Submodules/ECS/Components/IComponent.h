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
	class NLE_API IComponent
	{
	protected:
		static uint32_t m_NextID;
	};

	template <typename T>
	class NLE_API Component : public IComponent
	{
	public:

		static constexpr bool AllowMultiple = false; // default is false
	
		static uint32_t GetID()
		{
			static uint32_t id = ++m_NextID;
			return id;
		}

		
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:
	};

	struct BaseComponent 
	{
		virtual ~BaseComponent() = default;
		virtual const std::string Name() const = 0;
		virtual const uint32_t ID() const = 0;

		BaseComponent() = default;

		bool m_Enabled = true;
		bool m_SerializeToScene = true;
	};



	inline bool IsValidRuntimeIndex(uint32_t index)
	{
		return index != UINT32_MAX;
	}


#define DEFINE_FLAG_SET(Name, ...)                        \
struct Name                                               \
{                                                         \
    union                                                 \
    {                                                     \
        struct { __VA_ARGS__ };                           \
        uint8_t m_Flags = 0;                              \
    };                                                    \
                                                          \
    constexpr Name() = default;                           \
    constexpr explicit Name(uint8_t flags) : m_Flags(flags) {} \
                                                          \
    void Set(uint8_t mask, bool value = true)             \
    {                                                     \
        if (value) m_Flags |= mask;                       \
        else m_Flags &= ~mask;                            \
    }                                                     \
    bool IsSet(uint8_t mask) const { return (m_Flags & mask) != 0; } \
    void Clear(uint8_t mask) { m_Flags &= ~mask; }        \
    void Reset() { m_Flags = 0; }                         \
};



	enum ComponentFlags : uint8_t
	{
		ComponentFlags_Enabled = 1 << 0,
		ComponentFlags_Serialized = 1 << 1,
	};

	DEFINE_FLAG_SET(ComponentFlagSet,
		uint8_t m_Enabled : 1;
	uint8_t m_Serialized : 1;
	uint8_t m_Reserved : 6;
		);


}