#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Texture.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/
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
class Texture;

namespace NULLENGINE
{
	class Texture
	{
	public:
		Texture(const std::string& filePath);
		Texture() = default;
		void Bind() const ;
		void BindUnit(uint32_t slot) const ;
		void Unbind() const;

		unsigned int GetID() const;
		~Texture();

	private:
		unsigned int m_TextureID;

		int m_Width, m_Height, m_NrChannels;

		unsigned char* m_Data;

		std::string m_FilePath;

	};


}