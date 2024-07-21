#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	JsonWrapper.h
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
    class JsonWrapper {
    public:
        JsonWrapper(const nlohmann::json& jsonData);

        glm::vec2 GetVec2(const std::string& key, const glm::vec2& defaultValue = { 0.0f, 0.0f }) const;
        glm::vec3 GetVec3(const std::string& key, const glm::vec3& defaultValue = { 0.0f, 0.0f, 0.0f }) const;
        glm::vec4 GetVec4(const std::string& key, const glm::vec4& defaultValue = { 0.0f, 0.0f, 0.0f, 0.0f }) const;

        int GetInt(const std::string& key, int defaultValue = 0) const;
        float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
        bool GetBool(const std::string& key, bool defaultValue = false) const;
        std::string GetString(const std::string& key, const std::string& defaultValue = "") const;

        bool HasData(const std::string& key) const;

        bool Empty() const;

    private:
        const nlohmann::json& data;
    };

}