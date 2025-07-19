#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	class SpriteSource;  // Forward declaration of Texture
	class Mesh;  // Forward declaration of Texture

    //struct SpriteComponent : public BaseComponent
    //{
    //    unsigned int m_FrameIndex;
    //    SpriteSource* m_SpriteSource;
    //    SpriteSource* m_EmissiveSpriteSource = nullptr;
    //    Mesh* m_Mesh;
    //    glm::vec4 m_Color;
    //    glm::vec4 m_EmissiveColor;   // new emissive color (default black/no emission)
    //    float m_EmissiveStrength;    // new emissive strength multiplier
    //    std::string m_ShaderName;
    //    float m_Thickness;
    //    float m_Fade;

    //    // Constructor with default values, added emissiveColor and emissiveStrength
    //    SpriteComponent(
    //        unsigned int frameIndex = 0,
    //        SpriteSource* spriteSource = nullptr,
    //        SpriteSource* emissiveSpriteSource = nullptr,
    //        Mesh* mesh = nullptr,
    //        glm::vec4 color = glm::vec4(1.0f),
    //        glm::vec4 emissiveColor = glm::vec4(0.0f),    // no emission by default
    //        float emissiveStrength = 0.0f,                // no emission strength by default
    //        std::string shaderName = "default",
    //        float thickness = 1.0f,
    //        float fade = 0.005f)
    //        : m_FrameIndex(frameIndex),
    //        m_EmissiveSpriteSource(emissiveSpriteSource),
    //        m_SpriteSource(spriteSource),
    //        m_Mesh(mesh),
    //        m_Color(color),
    //        m_EmissiveColor(emissiveColor),
    //        m_EmissiveStrength(emissiveStrength),
    //        m_ShaderName(shaderName),
    //        m_Thickness(thickness),
    //        m_Fade(fade)
    //    {}

    //    const std::string Name() const { return Component<SpriteComponent>::TypeName(); }
    //    const uint32_t ID() const { return Component<SpriteComponent>::GetID(); }
    //};




    struct SpriteComponent
    {
        glm::vec4      m_Color = glm::vec4(1.0f);                       // 16 bytes, align 16
        glm::vec4      m_EmissiveColor = glm::vec4(1.0f);               // 16 bytes, align 16

        uint32_t       m_FrameIndex = 0;                  // 4 bytes
        uint32_t       m_SpriteSourceNameID = 0;          // 4 bytes
        uint32_t       m_EmissiveSpriteSourceNameID = 0;  // 4 bytes
        uint32_t       m_MeshNameID = 0;                   // 4 bytes

        float          m_EmissiveStrength = 0.0f;             // 4 bytes
        float          m_Thickness = 1.0f;                     // 4 bytes
        float          m_Fade = 0.005f;                          // 4 bytes

        ComponentFlagSet  m_ComponentFlags;                         // 1 byte
    };



}