#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	SpriteSource.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Texture.h"


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
	class NLE_API SpriteSource
	{
	public:
		SpriteSource(const std::string name, int numCols, int numRows);

		~SpriteSource();

		glm::vec2 GetUV(unsigned int frameIndex) const;
		glm::vec2 GetSize() const;

		int& Rows();
		int& Cols();


		const int& GetRows();
		const int& GetCols();
		
		int GetFrameCount() { return m_Rows * m_Cols; }

		Texture* GetTexture() const { return m_Texture; }

	private:
		// The dimensions of the sprite sheet.
		int	m_Rows;
		int	m_Cols;

		// Pointer to a texture created using the NUll Engine.
		Texture* m_Texture;
	};

}



