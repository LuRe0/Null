
//------------------------------------------------------------------------------
//
// File Name:	SpriteSource.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "SpriteSource.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	//SpriteSource::SpriteSource()
	//{
	//}



	SpriteSource::SpriteSource(int numCols, int numRows, Texture* pTexture) : m_Rows(numRows), m_Cols(numCols), m_Texture(pTexture)
	{

	}

	SpriteSource::~SpriteSource()
	{
	}

	glm::vec2 SpriteSource::GetUV(unsigned int frameIndex) const
	{
		float uSize = 1.0f / m_Cols;
		float vSize = 1.0f / m_Rows;


		float u = uSize * (frameIndex % m_Cols);
		float v = vSize * (frameIndex / m_Rows);


		return glm::vec2(u, v);
	}
}
