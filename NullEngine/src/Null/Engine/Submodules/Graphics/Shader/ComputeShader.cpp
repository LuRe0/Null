#include "ComputeShader.h"

//------------------------------------------------------------------------------
//
// File Name:	Shader.cpp
// Author(s):	Anthon Rem_RendererID
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ComputeShader.h"
#include "glad/glad.h"
#include "glm/glm.hpp"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

#define GL(function) \
    do { \
        while (glGetError() != GL_NO_ERROR); \
        function; \
        GLenum glError = glGetError(); \
        if (glError != GL_NO_ERROR) { \
            NLE_CORE_ASSERT(false, "OpenGL Error [{0}]", glError); \
        } \
    } while(0)

	ComputeShader::ComputeShader(const std::string& name)
		: Shader()
	{
		m_Name = name;

		std::string computePath = "../Assets/Shaders/ShaderScripts/" + name + "C.glsl";

		std::ifstream cShaderFile(computePath);
		if (!cShaderFile.is_open()) {
			NLE_CORE_ERROR("Failed to open compute shader: {0}", computePath);
			return;
		}

		std::unordered_set<std::string> includedFiles;
		std::string computeCode = PreprocessShader(computePath, includedFiles);

		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
		const char* source = computeCode.c_str();
		glShaderSource(computeShader, 1, &source, nullptr);
		glCompileShader(computeShader);

		GLint isCompiled = 0;
		glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(computeShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(computeShader);
			NLE_CORE_ERROR("Compute Shader compilation error: {0}", infoLog.data());
			NLE_CORE_ASSERT(isCompiled, "Compute Shader compilation failed!");
			return;
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, computeShader);
		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(m_RendererID);
			glDeleteShader(computeShader);

			NLE_CORE_ERROR("Compute Shader link error: {0}", infoLog.data());
			NLE_CORE_ASSERT(isLinked, "Compute Shader linkage failed!");
			return;
		}

		glDetachShader(m_RendererID, computeShader);
	}

	void ComputeShader::Dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) const
	{
		GL(glDispatchCompute(groupsX, groupsY, groupsZ));
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
	}

	std::string ComputeShader::PreprocessShader(const std::string& filePath, std::unordered_set<std::string>& includedFiles)
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			NLE_CORE_ERROR("Failed to open shader: {0}", filePath);
			return "";
		}

		std::string result;
		std::string line;
		std::filesystem::path baseDir = std::filesystem::path(filePath).parent_path();

		while (std::getline(file, line))
		{
			if (line.find("#include") != std::string::npos)
			{
				size_t start = line.find('"') + 1;
				size_t end = line.find_last_of('"');
				std::string includeFile = line.substr(start, end - start);
				std::string includePath = (baseDir / includeFile).generic_string();

				if (includedFiles.find(includePath) == includedFiles.end())
				{
					includedFiles.insert(includePath);
					result += PreprocessShader(includePath, includedFiles);
				}
			}
			else
			{
				result += line + "\n";
			}
		}

		return result;
	}





}
