
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
    JsonReader::JsonReader(const nlohmann::json& jsonData) : data(jsonData) {}

    glm::vec2 JsonReader::GetVec2(const std::string& key, const glm::vec2& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1] };
        }
        return defaultValue;
    }

    glm::vec3 JsonReader::GetVec3(const std::string& key, const glm::vec3& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1], data[key][2] };
        }
        return defaultValue;
    }

    glm::vec4 JsonReader::GetVec4(const std::string& key, const glm::vec4& defaultValue) const {
        if (data.contains(key)) {
            return { data[key][0], data[key][1], data[key][2], data[key][3] };
        }
        return defaultValue;
    }

    int JsonReader::GetInt(const std::string& key, int defaultValue) const {
        return data.value(key, defaultValue);
    }

    float JsonReader::GetFloat(const std::string& key, float defaultValue) const {
        return data.value(key, defaultValue);
    }

    bool JsonReader::GetBool(const std::string& key, bool defaultValue) const {
        return data.value(key, defaultValue);
    }

    std::string JsonReader::GetString(const std::string& key, const std::string& defaultValue) const {
        return data.value(key, defaultValue);
    }

    bool JsonReader::HasData(const std::string& key) const
    {
        return data.contains(key);
    }

    bool JsonReader::Empty() const
    {
        return data.empty();
    }




    void JsonWriter::SetVec2(const std::string& key, const glm::vec2& value) {
        data[key] = { value.x, value.y };
    }

    void JsonWriter::SetVec3(const std::string& key, const glm::vec3& value) {
        data[key] = { value.x, value.y, value.z };
    }

    void JsonWriter::SetVec4(const std::string& key, const glm::vec4& value) {
        data[key] = { value.x, value.y, value.z, value.w };
    }


    bool JsonWriter::HasData(const std::string& key) const
    {
        return data.contains(key);
    }

    bool JsonWriter::Empty() const
    {
        return data.empty();
    }

}
