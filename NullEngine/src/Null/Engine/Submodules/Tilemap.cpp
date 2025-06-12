
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Tilemap.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/QuadInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/TilemapBatchRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
//#include "Null/Engine/Modules/NTilesetManager.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Tilemap::Tilemap(const std::string& filename, const std::string& tilemapName)
        : m_MapWidth(0),
        m_TilemapName(tilemapName),
        m_MapHeight(0),
        m_TileWidth(0),
        m_TileHeight(0),
        m_Layers(),
        mTilesets()
	{
        const std::string path = "../Assets/Tilemaps/" + filename + ".tmj";
        std::ifstream file(path);
        if (!file.is_open()) {
            NLE_CORE_ERROR("Failed to open Tilemap file: {0}", path);
            return;
        }

        nlohmann::json jsonData;
        file >> jsonData;

        m_MapWidth = jsonData["width"];
        m_MapHeight = jsonData["height"];
        m_TileHeight = jsonData["tilewidth"];
        m_TileHeight = jsonData["tileheight"];

        for (const auto& layerJson : jsonData["layers"])
        {
            TileLayer layer;
            layer.name = layerJson["name"].get<std::string>();
            layer.id = layerJson["id"].get<int>();
            layer.width = layerJson["width"].get<int>();
            layer.height = layerJson["height"].get<int>();
            layer.opacity = layerJson["opacity"].get<float>();
            layer.visible = layerJson["visible"].get<bool>();
            layer.data = layerJson["data"].get<std::vector<int>>();

            AddLayer(layer);
        }

        if (jsonData.contains("tilesets"))
        {
            for (const auto& tilesetJson : jsonData["tilesets"])
            {
                int firstGid = tilesetJson["firstgid"];
                std::string source = tilesetJson["source"];

                // Extract just the filename without extension
                std::filesystem::path tilesetPath(source);
                std::string tilesetName = tilesetPath.stem().string();

                // Get the shared Tileset object 
                NTilesetManager* tilesetManager = NEngine::Instance().Get<NTilesetManager>();

                const Tileset* tileset = tilesetManager->Get(tilesetName);
                if (!tileset)
                {
                    NLE_CORE_WARN("Tileset '{}' not found!", tilesetName);
                    continue;
                }

                TilemapTilesetEntry entry;
                entry.tileset = tileset;
                entry.firstGid = firstGid;

                mTilesets.push_back(entry);
            }
        }
	}

    void Tilemap::AddLayer(const TileLayer& layer)
    {
        m_Layers.push_back(layer);
    }


    const TilemapTilesetEntry& Tilemap::GetTilesetEntryForTileID(int tileID) const
    {
        for (const auto& tsEntry : mTilesets)
        {
            int firstGid = tsEntry.firstGid;
            int lastGid = firstGid + tsEntry.tileset->GetTileCount() - 1;
            if (tileID >= firstGid && tileID <= lastGid)
                return tsEntry;
        }

        NLE_CORE_THROW("Tileset entry not found for tileID: {}", tileID);
    }

    void Tilemap::CreateTilemap(float tileScale, EntityID id)
    {
        NSceneManager* sceneMan = NEngine::Instance().Get<NSceneManager>();
        NRenderer* renderer = NEngine::Instance().Get<NRenderer>();
        NMeshManager* meshMan = NEngine::Instance().Get<NMeshManager>();
        Mesh* mesh = meshMan->Get("Quad");

        const std::size_t max_tiles = m_MapWidth * m_MapHeight * m_Layers.size();

        TilemapBatchRenderer<Instance, QuadInstanceMesh>* batcher = renderer->AddBatcher<TilemapBatchRenderer<Instance, QuadInstanceMesh>>("Tilemap", max_tiles);

        for (const auto& layer : m_Layers)
        {
            // Check if layer is visible
            if (!layer.visible)
                continue;

            for (int i = 0; i < layer.width * layer.height; ++i)
            {
                int tileID = layer.data[i];
                if (tileID == 0)
                    continue; // empty tile

                // Find the tileset containing this tileID
                const TilemapTilesetEntry& tsEntry = GetTilesetEntryForTileID(tileID);

                int tileFrameIndex = tileID - tsEntry.firstGid;

                SpriteSource* spriteSrc = tsEntry.tileset->GetSpriteSrc();
                if (!spriteSrc)
                {
                    NLE_CORE_WARN("Could not locate Sprite Source in Tileset {0}", tsEntry.tileset->GetName());
                    continue;
                }

                glm::vec2 position = GetPosition(layer.width, i);

                // Convert to world position (assuming y-axis down)
                glm::mat4 transform = CalculateTransform(tsEntry.tileset, glm::vec2(position.x, m_MapHeight - position.y), layer.id*10, tileScale);

                Scene* currScene = sceneMan->GetCurrentScene();
                //// Create Entity and assign components
                //Entity tile = currScene->GetEntity(currScene->LoadArchetype("Tile"));

                //if (tile.Has<TransformComponent>())
                //{
                //    TransformComponent& trans = tile.Get<TransformComponent>();
                //    trans.m_Translation = glm::vec3(position.x * tsEntry.tileset->GetTileWidth() * tileScale, position.y* tsEntry.tileset->GetTileHeight() * tileScale, -layer.id);
                //    trans.m_Scale = glm::vec3(tsEntry.tileset->GetTileWidth() * tileScale, tsEntry.tileset->GetTileHeight() * tileScale, 0);
                //    trans.m_TransformMatrix = transform;
                //}
                //if (tile.Has<SpriteComponent>())
                //{
                //    SpriteComponent& sprite = tile.Get<SpriteComponent>();
                //    sprite.m_SpriteSource = spriteSrc;
                //    sprite.m_FrameIndex = tileFrameIndex;
                //}

                ElementData renderdata(transform, mesh, spriteSrc, glm::vec4(1.0f,1.0f,1.0f,1.0f) , "",
                    tileFrameIndex, id, 1.0f, 1.0f, RenderData::INSTANCED, -layer.id);
      
                batcher->AddInstance(renderdata);
            }
        }
    }


    inline glm::vec2 Tilemap::GetPosition(int width, int index) const
    {
        return glm::vec2(index % width, index / width);
    }

    glm::mat4 Tilemap::CalculateTransform(const Tileset* tileset, glm::vec2 position, float depth, float tileScale)
    {
        glm::vec3 translation  = glm::vec3(position.x * tileset->GetTileWidth() * tileScale, position.y * tileset->GetTileHeight() * tileScale, -depth);
        glm::vec3 scale = glm::vec3(tileset->GetTileWidth() * tileScale, tileset->GetTileHeight() * tileScale, 0);
        glm::vec3 rotation(0.0f);

        
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
        // Calculate rotation matrix (assuming Euler angles in radians)
        glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(rotation)));

        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 transform = translationMatrix * rotationMatrix * scaleMatrix;

        return transform;
    }


}
