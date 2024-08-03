#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	VariableContainer.h
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
    class NLE_API VariableContainer
    {
    public:
        template <typename T>
        void set_value(std::string key, const T& value);

        template <typename T>
        T& get_value(std::string key) const;
    private:
        std::unordered_map<std::string, std::any> data;
    };

    template<typename T>
    inline void VariableContainer::set_value(std::string key, const T& value)
    {
        NLE_CORE_ASSERT(std::is_copy_constructible<T>::value, "Attempting to add non-copy-constructible type into VariableContainer");

        data[key] = value;
    }

    template<typename T>
    inline T& VariableContainer::get_value(std::string key) const
    {
        NLE_CORE_ASSERT(!data.contains(key), "Variable {0} not registered", key);

        T& value = std::any_cast<T>(data.at(key));

        return value;
    }


}