
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
#include "Tileset.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Tileset::Tileset() 
		: m_Name("")
		, m_Columns(0)
		, m_ImageWidth(0)
		, m_ImageHeight()
	    , m_TileWidth(0)
	    , m_TileHeight(0)
	{

	}

	Tileset::Tileset(const std::string& name)
		: m_Name(name)
	{
		std::string filePath = "../Assets/Tilesets/" + name + ".tsj";

		std::ifstream file(filePath);
		if (!file.is_open()) {
			NLE_ERROR("Tileset: Failed to open file '{}'", filePath);
			return;
		}

		JSON json;
		try {
			file >> json;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Tileset: Failed to parse '{}': {}", filePath, e.what());
			return;
		}

		LoadFromJson(json);
	}

	void Tileset::Create(const std::string& inName, unsigned int inCols, float inImageW, float inImageH, float inTileW, float inTileH, int inFirsGID)
	{
		m_Name = inName;
		m_Columns = inCols;
		m_ImageWidth = inImageW;
		m_ImageHeight = inImageH;
		m_TileWidth = inTileW;
		m_TileHeight = inTileH;
		m_Rows = static_cast<int>(inImageH / inTileH);
	}

	void Tileset::LoadFromJson(const JSON& json)
	{
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();

		m_Name = json["name"];
		m_Columns = json["columns"];
		m_TileCount = json["tilecount"];
		m_TileWidth = json["tilewidth"];
		m_TileHeight = json["tileheight"];
		m_ImageWidth = json["imagewidth"];
		m_ImageHeight = json["imageheight"];
		m_Rows = m_TileCount / m_Columns;

		std::string imagePath = json["image"];
		std::filesystem::path path(imagePath);
		m_SpriteSrcName = path.stem().string();

		if (!m_SpriteSrcName.empty())
			m_SpriteSrc = spritesrcManager->Create(m_SpriteSrcName, m_Rows, m_Columns);
		else
			m_SpriteSrc = nullptr;
	}

	void Tileset::Load()
	{
		std::string filePath = std::string("../Assets/Scenes/Paths/") + m_Name + std::string(".tsj");

		std::ifstream inputFile(filePath);
		if (!inputFile.is_open())
		{
			NLE_CORE_ERROR("Error: Could not open file {}", filePath);
			return;
		}

		JSON tilesetData;
		try {
			inputFile >> tilesetData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Error: JSON parsing failed: {0}", e.what());
			return;
		}

		// Read values from JSON and store them in the member variables
		try 
		{

			m_TileCount = (tilesetData.at("tilecount").get<unsigned int>());
			m_Columns = tilesetData.at("columns").get<unsigned int>();
			m_Rows = static_cast<unsigned int>(tilesetData.at("tilecount").get<int>() / m_Columns);
			m_ImageWidth = tilesetData.at("imagewidth").get<float>();
			m_ImageHeight = tilesetData.at("imageheight").get<float>();
			m_TileWidth = tilesetData.at("tilewidth").get<float>();
			m_TileHeight = tilesetData.at("tileheight").get<float>();

			NLE_CORE_INFO("Tileset '{}' loaded successfully", m_Name);
		}
		catch (const std::exception& e) {
			NLE_ERROR("Error: Failed to extract tileset data: {0}", e.what());
		}
	}

	
}
