
//------------------------------------------------------------------------------
//
// File Name:	GLHelper.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "GLHelper.h"
#include "glad/glad.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	unsigned int GLHelper::StringToGLenum(const std::string& name)
	{
		const auto& map = GetMap();
		auto it = map.find(name);
		NLE_CORE_ASSERT(it != map.end(), "Unknown GL constant string: {}", name);
		return it->second;
	}

	const std::unordered_map<std::string, unsigned int>& GLHelper::GetMap()
	{
		static std::unordered_map<std::string, unsigned int> map = 
		{
			{"GL_RGBA8", GL_RGBA8},
			{"GL_RGB8", GL_RGB8},
			{"GL_RGBA16F", GL_RGBA16F},
			{"GL_RGB16F", GL_RGB16F},
			{"GL_R32F", GL_R32F},
			{"GL_R32I", GL_R32I},
			{"GL_R8", GL_R8},

			{"GL_DEPTH_COMPONENT24", GL_DEPTH_COMPONENT24},
			{"GL_DEPTH_COMPONENT32", GL_DEPTH_COMPONENT32},
			{"GL_DEPTH_COMPONENT32F", GL_DEPTH_COMPONENT32F},
			{"GL_DEPTH24_STENCIL8", GL_DEPTH24_STENCIL8},
			{"GL_DEPTH_STENCIL", GL_DEPTH_STENCIL},

			{"GL_RGBA", GL_RGBA},
			{"GL_RGB", GL_RGB},
			{"GL_RED", GL_RED},
			{"GL_RED_INTEGER", GL_RED_INTEGER},
			{"GL_DEPTH_COMPONENT", GL_DEPTH_COMPONENT},

			{"GL_UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
			{"GL_FLOAT", GL_FLOAT},
			{"GL_INT", GL_INT},
			{"GL_UNSIGNED_INT_24_8", GL_UNSIGNED_INT_24_8},

			{"GL_COLOR_ATTACHMENT0", GL_COLOR_ATTACHMENT0},
			{"GL_COLOR_ATTACHMENT1", GL_COLOR_ATTACHMENT1},
			{"GL_COLOR_ATTACHMENT2", GL_COLOR_ATTACHMENT2},
			{"GL_COLOR_ATTACHMENT3", GL_COLOR_ATTACHMENT3},
			{"GL_COLOR_ATTACHMENT4", GL_COLOR_ATTACHMENT4},
			{"GL_COLOR_ATTACHMENT5", GL_COLOR_ATTACHMENT5},
			{"GL_COLOR_ATTACHMENT6", GL_COLOR_ATTACHMENT6},
			{"GL_COLOR_ATTACHMENT7", GL_COLOR_ATTACHMENT7},

			{"GL_DEPTH_ATTACHMENT", GL_DEPTH_ATTACHMENT},
			{"GL_STENCIL_ATTACHMENT", GL_STENCIL_ATTACHMENT},
			{"GL_DEPTH_STENCIL_ATTACHMENT", GL_DEPTH_STENCIL_ATTACHMENT},

			{"GL_TEXTURE_2D", GL_TEXTURE_2D},
			{"GL_FRAMEBUFFER", GL_FRAMEBUFFER},
			{"GL_RENDERBUFFER", GL_RENDERBUFFER},

			{"GL_LINEAR", GL_LINEAR},
			{"GL_CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE},

			{"GL_COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT},
			{"GL_DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT},
			{"GL_STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT},

			{"GL_FRAMEBUFFER_COMPLETE", GL_FRAMEBUFFER_COMPLETE},
			{"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT},
			{"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT},
			{"GL_FRAMEBUFFER_UNSUPPORTED", GL_FRAMEBUFFER_UNSUPPORTED},
			{"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER},
			{"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER},
		};
		return map;
	}
}
