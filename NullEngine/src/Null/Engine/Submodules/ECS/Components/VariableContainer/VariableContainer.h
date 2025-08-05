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
        VariableContainer();
        ~VariableContainer();

        VariableContainer(const VariableContainer&) = delete;
        VariableContainer& operator=(const VariableContainer&) = delete;

        template <typename T>
        void set_value(std::string key, const T& value)
        {
            data[std::move(key)] = value;
        }

        //template <typename T>
        //T& get_value(std::string key) const
        //{
        //    if (!data.contains(key))
        //        NLE_CORE_ASSERT(false, "Variable {0} not registered", key);

        //    return std::any_cast<T&>(data.at(key));
        //}

    private:
        std::unordered_map<std::string, std::any> data;

    };
}