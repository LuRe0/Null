
//------------------------------------------------------------------------------
//
// File Name:	Texture.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Texture.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Texture::Texture(const std::string& filePath)
	{
        m_FilePath = std::string("../Assets/Textures/") + filePath + std::string(".png");

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        // Load the texture image
        //stbi_set_flip_vertically_on_load(true);
        m_Data = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);
        if (m_Data) {
            GLenum format = (m_NrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_Data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else 
        {
            NLE_CORE_ERROR("Failed to load texture: {0}", filePath);
        }


        stbi_image_free(m_Data);

        glBindTexture(GL_TEXTURE_2D, 0);
	}

    void Texture::Bind() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture::Unbind() const
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    unsigned int Texture::GetID() const
    {
        return m_TextureID;
    }

    Texture::~Texture()
	{
        glDeleteTextures(1, &m_TextureID);
	}
}
