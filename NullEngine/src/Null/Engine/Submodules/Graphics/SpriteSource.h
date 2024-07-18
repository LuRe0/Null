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
		SpriteSource(int numCols, int numRows, Texture* pTexture);

		~SpriteSource();

		glm::vec2 GetUV(unsigned int frameIndex) const;
		
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



