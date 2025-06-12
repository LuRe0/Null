#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Tileset.h
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
	class SpriteSource;  // Forward declaration of spriteSource

	class NLE_API Tileset
	{
	public:
		Tileset();
		Tileset(const std::string& name);
		void Create(const std::string& inName, unsigned int inCols, float inImageW, float inImageH, float inTileW, float inTileH, int inFirsGID);

		void Load();

		void LoadFromJson(const JSON& json);


		inline unsigned int GetTileCount(void) const { return m_TileCount; }
		inline float GetTileWidth(void) const { return m_TileWidth; }
		inline float GetTileHeight(void) const { return m_TileHeight; }
		inline SpriteSource* GetSpriteSrc(void) const { return m_SpriteSrc; }
		inline std::string_view GetName(void) const { return m_Name; }

	private:
		std::string m_Name;
		std::string m_SpriteSrcName;
		unsigned int m_Rows;
		unsigned int m_Columns;
		unsigned int m_TileCount;
		float m_ImageWidth;
		float m_ImageHeight;
		float m_TileWidth;
		float m_TileHeight;
		SpriteSource* m_SpriteSrc;
	};

}