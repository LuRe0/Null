#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Tilemap.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

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
	class Tileset;
	using EntityID = uint32_t;

	struct NLE_API TileLayer 
    {
		std::string name;
		int id = 0;
		int width = 0;
		int height = 0;
		float opacity = 1.0f;
		bool visible = true;
		std::vector<int> data;
	};

    struct TilemapTilesetEntry
    {
        const Tileset* tileset;
        int firstGid;
    };

    class NLE_API Tilemap
    {
    public:
        Tilemap(const std::string& filename, const std::string& tilemapName);

        // Layer-related
        void AddLayer(const TileLayer& layer);

        const TilemapTilesetEntry& GetTilesetEntryForTileID(int tileID) const;

        void CreateTilemap(float tileScale, EntityID id);
    private:
        int m_MapWidth;
        int m_MapHeight;
        int m_TileWidth;
        int m_TileHeight;

        float m_TileScale = 1.0f;

        std::string m_TilemapName = "";
        std::vector<TileLayer> m_Layers;
        std::vector<TilemapTilesetEntry> mTilesets;


        inline glm::vec2 GetPosition(int width, int index) const;

        glm::mat4 CalculateTransform(const Tileset* tileset, glm::vec2 xy, float depth, float tilescale);

        int CalculateFrameIndex(int index, int rows, int cols);

    };


}