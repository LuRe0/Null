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
    class JsonReader {
    public:
        JsonReader(const nlohmann::json& jsonData);

        glm::vec2 GetVec2(const std::string& key, const glm::vec2& defaultValue = { 0.0f, 0.0f }) const;
        glm::vec3 GetVec3(const std::string& key, const glm::vec3& defaultValue = { 0.0f, 0.0f, 0.0f }) const;
        glm::vec4 GetVec4(const std::string& key, const glm::vec4& defaultValue = { 0.0f, 0.0f, 0.0f, 0.0f }) const;

        int GetInt(const std::string& key, int defaultValue = 0) const;
        uint8_t GetUInt8(const std::string& key, uint8_t defaultValue) const;
        float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
        bool GetBool(const std::string& key, bool defaultValue = false) const;
        std::string GetString(const std::string& key, const std::string& defaultValue = "") const;

        //std::vector<float> GetFloatArray(const std::string& key, size_t expectedSize = 0) const;

        JsonReader GetJSONObject(const std::string& key) const {
            if (data.contains(key) && data[key].is_object()) {
                return JsonReader(data[key]);
            }
            // Return an empty JsonReader if key is missing or not an object
            static const nlohmann::json emptyObject = nlohmann::json::object();
            return JsonReader(emptyObject);
        }

        nlohmann::json GetJSONArray(const std::string& key) const
        {
            if (HasData(key) && data[key].is_array())
            {
                return data[key];
            }
            return nlohmann::json::array(); // empty array fallback
        }


        bool HasData(const std::string& key) const;

        bool Empty() const;

        std::vector<float> GetFloatArray(const std::string& key, const std::vector<float>& defaultValues) const;


        template <typename T>
        std::vector<T> GetArray(const std::string& key, const std::vector<T>& defaultValues) const
        {
            if (data.contains(key) && data[key].is_array())
            {
                std::vector<T> result;
                for (const auto& elem : data[key])
                    result.push_back(elem.get<T>());

                if (result.size() < defaultValues.size())
                {
                    size_t currentSize = result.size();
                    result.resize(defaultValues.size());
                    for (size_t i = currentSize; i < defaultValues.size(); i++)
                        result[i] = defaultValues[i];
                }
                return result;
            }
            return defaultValues;
        }


    private:
        const nlohmann::json& data;

        
    };


    class JsonWriter {
    public:
        JsonWriter(nlohmann::json& jsonData);


        // Setter methods
        void SetVec2(const std::string& key, const glm::vec2& value);
        void SetVec3(const std::string& key, const glm::vec3& value);
        void SetVec4(const std::string& key, const glm::vec4& value);

        template <typename T>
        void SetValue(const std::string& key, T value);


        bool HasData(const std::string& key) const;

        bool Empty() const;

    private:
        nlohmann::json& data;


    };

    template<typename T>
    inline void JsonWriter::SetValue(const std::string& key, T value)
    {
        data[key] = value;
    }

}