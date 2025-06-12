#pragma once
#include "IComponent.h"
#include "Null/Engine/Submodules/Tilemap.h"
#include <unordered_map>
#include <string.h>

namespace NULLENGINE
{
	class SpriteSource;  // Forward declaration of Texture
	class Mesh;  // Forward declaration of Texture

    struct TilemapComponent : public BaseComponent
    {
		std::string m_Name;
		std::string m_PathName;
        float m_TileScale;
        std::unique_ptr<Tilemap> m_Tilemap;

        TilemapComponent() = default;

        TilemapComponent(const std::string inName, const std::string inPathName, float inTileScale)
            : m_Name(inName)
            , m_PathName(inPathName)
            , m_TileScale(inTileScale)
        {}


        TilemapComponent(const TilemapComponent& other)
            : m_Name(other.m_Name)
            , m_PathName(other.m_PathName)
            , m_TileScale(other.m_TileScale)
            , m_Tilemap(nullptr)
        {}


        TilemapComponent& operator=(const TilemapComponent& other)
        {
            if (this != &other)
            {
                m_Name = other.m_Name;
                m_PathName = other.m_PathName;
                m_TileScale = other.m_TileScale;
                m_Tilemap.reset();  
            }
            return *this;
        }

        // Move operations can be defaulted
        TilemapComponent(TilemapComponent&&) = default;
        TilemapComponent& operator=(TilemapComponent&&) = default;

        const std::string Name() const { return Component<TilemapComponent>::TypeName(); }
        const uint32_t ID() const { return Component<TilemapComponent>::GetID(); }
    };

}