#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Shader.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
//#include "glad/glad.h"


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
	class Shader
	{
	public:
		Shader(const std::string& name);
		~Shader();

		void Bind() const ;
		void Unbind() const;
		uint32_t GetID() const;

		// utility uniform functions
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setFloat2fv(const std::string& name, const float* value) const;
		void setMat4(const std::string& name, const glm::mat4& value) const;
		void setVec4(const std::string& name, const glm::vec4& value) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;

		void setFullTransform(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const;
		void setTextureOffset(float uSize, float vSize)const;
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}