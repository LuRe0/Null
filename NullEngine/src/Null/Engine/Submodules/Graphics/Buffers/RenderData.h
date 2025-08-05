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
	using EntityID = uint32_t;

    struct RenderData
    {
        virtual ~RenderData() = default;

        enum RenderType
        {
            ELEMENT,
            INSTANCED,
        };
        //const std::vector<unsigned int> m_textureIDs;

        RenderType m_Type = RenderType::INSTANCED;
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
            tintColor(glm::vec4(1.0f)), shaderName(""), frameIndex(0), entity(0), thickness(0.0f), fade(0.0f), depth(1.0f)
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

    struct ParticleData : public RenderData
    {
        const SpriteSource* spriteSrc = nullptr;
        float depth = 0.0f;

        ParticleData() = default;

        ParticleData(const SpriteSource* inSrc, float z = 0.0f)
            : spriteSrc(inSrc), depth(z)
        {
            m_Type = RenderType::ELEMENT;
        }
    };

    struct ParticleDepthCompare {
        bool operator()(const std::unique_ptr<ParticleData>& a,
            const std::unique_ptr<ParticleData>& b) const
        {
            return a->depth < b->depth; // larger depth comes first
        }
    };

    struct ElementDepthCompare {
        bool operator()(const std::unique_ptr<ElementData>& a, const std::unique_ptr<ElementData>& b) const {
            return a->depth < b->depth;  // For sorting in descending order (back to front)
        }
    };

    //struct InstanceData : public RenderData
    //{
    //    const std::vector<DrawInstance> instances;


    //    // Default constructor
    //    InstanceData() = default;

    //    // Constructor that takes a vector of Instance
    //    InstanceData(const std::vector<DrawInstance>& instances) : instances(instances)
    //    {
    //        m_Type = RenderType::ELEMENT;
    //    }
    //};

    struct IRenderQueue {
        virtual ~IRenderQueue() = default;
        virtual bool Empty() const = 0;

        virtual void Push(std::unique_ptr<RenderData>&& data) = 0;

        virtual std::unique_ptr<RenderData> Pop() = 0;

        virtual void Clear() = 0;
    };


    enum class RenderCommandTypes: int
    {
        Opaque,
        Transparent,
        Emissive,
        Particles,
        UI,
        Debug,
        QUEUES,
    };

    template<typename T, typename Comparator>
    class RenderQueue : public IRenderQueue {
    public:
        using Ptr = std::unique_ptr<T>;

        void Push(std::unique_ptr<RenderData>&& data) override {
            // dynamic_cast to ensure type safety
            T* derived = dynamic_cast<T*>(data.get());
            if (!derived) {
                throw std::runtime_error("RenderQueue: bad data type pushed");
            }
            // Take ownership, release from original unique_ptr
            Ptr typedPtr(static_cast<T*>(data.release()));
            m_Queue.push(std::move(typedPtr));
        }

        bool Empty() const override {
            return m_Queue.empty();
        }

        std::unique_ptr<RenderData> Pop() override {
            Ptr top = std::move(const_cast<Ptr&>(m_Queue.top()));
            m_Queue.pop();
            return std::unique_ptr<RenderData>(std::move(top));
        }

        void Clear() override {
            while (!m_Queue.empty())
                m_Queue.pop();
        }

    private:
        std::priority_queue<Ptr, std::vector<Ptr>, Comparator> m_Queue;
    };


}