
//------------------------------------------------------------------------------
//
// File Name:	JsonWrapper.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "JsonWrapper.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    JsonWrapper::JsonWrapper(const nlohmann::json& jsonData) : data(jsonData) {}

    glm::vec2 JsonWrapper::GetVec2(const std::string& key, const glm::vec2& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1] };
        }
        return defaultValue;
    }

    glm::vec3 JsonWrapper::GetVec3(const std::string& key, const glm::vec3& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1], data[key][2] };
        }
        return defaultValue;
    }

    glm::vec4 JsonWrapper::GetVec4(const std::string& key, const glm::vec4& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1], data[key][2], data[key][3] };
        }
        return defaultValue;
    }

    int JsonWrapper::GetInt(const std::string& key, int defaultValue) const {
        return data.value(key, defaultValue);
    }

    float JsonWrapper::GetFloat(const std::string& key, float defaultValue) const {
        return data.value(key, defaultValue);
    }

    bool JsonWrapper::GetBool(const std::string& key, bool defaultValue) const {
        return data.value(key, defaultValue);
    }

    std::string JsonWrapper::GetString(const std::string& key, const std::string& defaultValue) const {
        return data.value(key, defaultValue);
    }

    bool JsonWrapper::HasData(const std::string& key) const
    {
        return data.contains(key);
    }

    bool JsonWrapper::Empty() const
    {
        return data.empty();
    }

}
