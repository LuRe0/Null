#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	RenderData.h
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
    class Mesh;
    class InstanceMesh;
    class SpriteSource;

    struct RenderData
    {
        virtual ~RenderData() = default;

        enum RenderType
        {
            ELEMENT,
            INSTANCED,
        };
        //const std::vector<unsigned int> m_textureIDs;

        RenderType m_Type;
    };

    struct ElementData : public RenderData
    {
        glm::mat4 model;                // 64 bytes
        const Mesh* mesh;   // 8 bytes (assuming 64-bit pointers)
        const SpriteSource* spriteSrc; // 8 bytes (assuming 64-bit pointers)
        glm::vec4 tintColor;            // 16 bytes
        std::string shaderName;         // 24 bytes (approx., depends on the implementation)
        unsigned int frameIndex;        // 4 bytes
        EntityID entity;
        float thickness;
        float fade;
        float depth;

        // Default constructor
        ElementData()
            : model(glm::mat4(1.0f)), mesh(nullptr), spriteSrc(nullptr),
            tintColor(glm::vec4(1.0f)), shaderName(""), frameIndex(0), entity(0)
        {
            m_Type = RenderType::ELEMENT;
        }

        // Parameterized constructor
        ElementData(const glm::mat4& model, const Mesh* mesh,
            const SpriteSource* spriteSrc, const glm::vec4& tintColor,
            const std::string& shaderName, unsigned int frameIndex, EntityID entity, float thick, float f, RenderType type, float d = 1.0f)
            : model(model), mesh(mesh), spriteSrc(spriteSrc), tintColor(tintColor),
            shaderName(shaderName), frameIndex(frameIndex), entity(entity), thickness(thick), fade(f), depth(d)
        {
            m_Type = type;
        }
    };


    struct InstanceData : public RenderData
    {
        const std::vector<Instance> instances;


        // Default constructor
        InstanceData() = default;

        // Constructor that takes a vector of Instance
        InstanceData(const std::vector<Instance>& instances) : instances(instances)
        {
            m_Type = RenderType::ELEMENT;
        }
    };
}