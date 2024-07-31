#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Framebuffer.h
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


namespace NULLENGINE
{

	class VBO;

	class Framebuffer
	{
	public:
		struct Format {
			unsigned int m_InternalFormat;
			unsigned int m_Format;
			unsigned int m_Type;
		};
		/**
			 * @brief Constructor to initialize the framebuffer with given width and height.
			 * @param width Width of the framebuffer.
			 * @param height Height of the framebuffer.
			 */
		Framebuffer(unsigned int width, unsigned int height);

		/**
		 * @brief Destructor to clean up the framebuffer.
		 */
		~Framebuffer();

		/**
		 * @brief Initializes the framebuffer.
		 */
		void Init();

		/**
		 * @brief Shuts down the framebuffer and releases resources.
		 */
		void Shutdown();

		/**
		 * @brief Binds the framebuffer.
		 */
		void Bind() const;

		/**
		 * @brief Unbinds the framebuffer.
		 */
		void Unbind() const;

		/**
		 * @brief Resizes the framebuffer to the given width and height.
		 * @param width New width of the framebuffer.
		 * @param height New height of the framebuffer.
		 */
		void Resize(unsigned int width, unsigned int height);

		/**
		 * @brief Clears the framebuffer render.
		 */
		void ClearRender() const;

		/**
		 * @brief Gets the color attachment at the specified index.
		 * @param index Index of the color attachment to get.
		 * @return The color attachment ID.
		 */
		unsigned int GetColorAttachment(unsigned int index = 0) const;

			/**
	   * @brief Gets the pixel id at the specified index.
	   * @param index Index of the color attachment to get.
	   * @return The pixel ID of the color attachment ID.
	   */
		int ReadPixels(unsigned int index, unsigned int x, unsigned int y) const;

		/**
		 * @brief Adds a new color attachment to the framebuffer.
		 * @param format The internal format of the texture (e.g., GL_RGB, GL_RGBA).
		 * @param attachmentType The attachment type (e.g., GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1).
		 *
		 * @details The format specifies the color components and their types. Common values are:
		 * - GL_RGB: 3 color components (red, green, blue).
		 * - GL_RGBA: 4 color components (red, green, blue, alpha).
		 *
		 * The attachment type specifies the role of the texture in the framebuffer. Common values are:
		 * - GL_COLOR_ATTACHMENT0: First color attachment.
		 * - GL_COLOR_ATTACHMENT1: Second color attachment.
		 */
		void AddColorAttachment(const std::vector<Format>& format);

		/**
		 * @brief Sets an existing color attachment at the specified index.
		 * @param index Index of the color attachment to set.
		 * @param format The internal format of the texture (e.g., GL_RGB, GL_RGBA).
		 * @param attachmentType The attachment type (e.g., GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1).
		 *
		 * @details The format specifies the color components and their types. Common values are:
		 * - GL_RGB: 3 color components (red, green, blue).
		 * - GL_RGBA: 4 color components (red, green, blue, alpha).
		 *
		 * The attachment type specifies the role of the texture in the framebuffer. Common values are:
		 * - GL_COLOR_ATTACHMENT0: First color attachment.
		 * - GL_COLOR_ATTACHMENT1: Second color attachment.
		 */
		void SetColorAttachment(unsigned int index, unsigned int format, unsigned int attachmentType);

		/**
		 * @brief Removes the color attachment at the specified index.
		 * @param index Index of the color attachment to remove.
		 */
		void RemoveColorAttachment(unsigned int index);


				/**
		 * @brief Clears the color attachment at the specified index with a given color.
		 * @param index Index of the color attachment to clear.
		 * @param color Pointer to the clear color data.
		 * @param format The format struct describing the color data.
		 */
		void ClearColorAttachment(unsigned int index, const void* value) const;

	private:
		void CreateFramebuffer(unsigned int width, unsigned int height);
		void DeleteFramebuffer();

		unsigned int m_FramebufferID;
		//unsigned int m_ColorAttachment;
		unsigned int m_DepthAttachment;
		unsigned int m_RenderbufferID;

		std::vector<unsigned int> m_ColorAttachments;

		std::vector<Format> m_Formats;

		float m_WinWidth;
		float m_WinHeight;
	};


}