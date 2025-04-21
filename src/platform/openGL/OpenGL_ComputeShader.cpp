#include "fmepch.h"
#include "OpenGL_ComputeShader.h"

namespace FMEditor {
	OpenGL_ComputeShader::OpenGL_ComputeShader(const char* path)
	{
		std::string computeCode;
		std::ifstream shaderFile;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			shaderFile.open(path);
			std::stringstream cShaderStream;

			cShaderStream << shaderFile.rdbuf();

			shaderFile.close();

			computeCode = cShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: read file error: %s", path);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: read file error: {0}", path);
			FME_DEBUG_ASSERT(0);
		}

		const char* cShaderCode = computeCode.c_str();

		////////////////////
		// compute shader //--------------------------------------
		////////////////////
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &cShaderCode, NULL);
		glCompileShader(computeShader);
		if (!CheckShaderError(computeShader)) {
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: %s", path);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: {0}", path);
			FME_DEBUG_ASSERT(0);
		}

		//////////
		// link //------------------------------------
		//////////
		m_ID = glCreateProgram();
		glAttachShader(m_ID, computeShader);
		glLinkProgram(m_ID);
		if (!CheckProgramError(m_ID, computeShader)) {
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: failed to link shader: %s", path);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: failed to link shader: {0}", path);
			FME_DEBUG_ASSERT(0);
		}

		glDetachShader(m_ID, computeShader);

		FME_LOG_INFO("[OpenGL_Shader.cpp]: shader successfully created: %s", path);
		FME_DEBUG_LOG_INFO("[OpenGL_Shader.cpp]: shader successfully created: {0}", path);
	}

	OpenGL_ComputeShader::~OpenGL_ComputeShader()
	{
		glDeleteProgram(m_ID);
	}

	void OpenGL_ComputeShader::Bind()
	{
		glUseProgram(m_ID);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void OpenGL_ComputeShader::Unbind()
	{
		glUseProgram(0);
	}

	bool OpenGL_ComputeShader::CheckShaderError(GLint shaderID)
	{
		GLint isCompiled = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);

			glDeleteShader(shaderID);

			FME_LOG_ERROR("[OpenGL_ComputeShader.cpp]: shader compile error: %s", infoLog);
			FME_DEBUG_LOG_ERROR("[OpenGL_ComputeShader.cpp]: shader compile error: {0}", infoLog);
			return false;
		}
		return true;
	}

	bool OpenGL_ComputeShader::CheckProgramError(GLint programID, GLint compID)
	{
		GLint isLinked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(programID, 1024, NULL, infoLog);

			glDeleteProgram(programID);
			glDeleteShader(compID);

			FME_LOG_ERROR("[OpenGL_ComputeShader.cpp]: shader link error: %s", infoLog);
			FME_DEBUG_LOG_ERROR("[OpenGL_ComputeShader.cpp]: shader link error: {0}", infoLog);
			return false;
		}
		return true;
	}
}
