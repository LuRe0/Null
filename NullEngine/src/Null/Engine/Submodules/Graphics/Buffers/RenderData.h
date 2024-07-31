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

class NULLENGINE::Mesh;
class NULLENGINE::SpriteSource;

struct RenderData
{
	const glm::mat4 model;           // 64 bytes
	const NULLENGINE::Mesh* mesh;              // 8 bytes (assuming 64-bit pointers)
	const NULLENGINE::SpriteSource* spriteSrc; // 8 bytes (assuming 64-bit pointers)
	const glm::vec4 tintColor;           // 16 bytes
	const std::string shaderName;            // 24 bytes (approx., depends on the implementation)
	const unsigned int frameIndex;                // 4 bytes
	EntityID entity;

	//const std::vector<unsigned int> m_textureIDs;
};