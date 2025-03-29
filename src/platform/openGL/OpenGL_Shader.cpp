#include "fmepch.h"
#include "OpenGL_Shader.h"
#include "core/Macros.h"

void checkOpenGLError() {
	GLenum errorCode = glGetError();
	if (errorCode != 0) {
		assert(false); 
	}
}

namespace FMEditor {


	OpenGL_Shader::OpenGL_Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: read file error: %s %s", vertexPath, fragmentPath);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: read file error: {0} {1}", vertexPath, fragmentPath);
			FME_DEBUG_ASSERT(0);
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		///////////////////
		// vertex shader //--------------------------------------
		///////////////////
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		if (!CheckShaderError(vertexShader)) {
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: %s", vertexPath);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: {0}", vertexPath);
			FME_DEBUG_ASSERT(0);
		}

		/////////////////////
		// fragment shader //------------------------------------
		/////////////////////
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		if (!CheckShaderError(fragmentShader)) {
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: %s", fragmentPath);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: failed to compile: {0}", fragmentPath);
			FME_DEBUG_ASSERT(0);
		}

		//////////
		// link //------------------------------------
		//////////
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);
		if (!CheckProgramError(m_ID, vertexShader, fragmentShader)) {
			FME_LOG_ERROR("[OpenGL_Shader.cpp]: failed to link shader: %s %s", vertexPath, fragmentPath);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: failed to link shader: {0} {1}", vertexPath, fragmentPath);
			FME_DEBUG_ASSERT(0);
		}

		glDetachShader(m_ID, vertexShader);
		glDetachShader(m_ID, fragmentShader);

		FME_LOG_INFO("[OpenGL_Shader.cpp]: shader successfully created: %s %s", vertexPath, fragmentPath);
		FME_DEBUG_LOG_INFO("[OpenGL_Shader.cpp]: shader successfully created: {0} {1}", vertexPath, fragmentPath);
	}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram(m_ID);
	}

	void OpenGL_Shader::Bind()
	{
		glUseProgram(m_ID);
	}

	void OpenGL_Shader::Unbind()
	{
		glUseProgram(0);
	}

	void OpenGL_Shader::setMat4(const char* name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, &mat[0][0]);
	}

	void OpenGL_Shader::setVec3(const char* name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_ID, name), x, y, z);
	}

	void OpenGL_Shader::setVec4(const char* name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(m_ID, name), x, y, z, w);
	}

	bool OpenGL_Shader::CheckShaderError(GLint shaderID)
	{
		GLint isCompiled = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);

			glDeleteShader(shaderID);

			FME_LOG_ERROR("[OpenGL_Shader.cpp]: shader compile error: %s", infoLog);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: shader compile error: {0}", infoLog);
			return false;
		}
		return true;
	}

	bool OpenGL_Shader::CheckProgramError(GLint programID, GLint vertexID, GLint fragID)
	{
		GLint isLinked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(programID, 1024, NULL, infoLog);

			glDeleteProgram(programID);
			glDeleteShader(vertexID);
			glDeleteShader(fragID);

			FME_LOG_ERROR("[OpenGL_Shader.cpp]: shader link error: %s", infoLog);
			FME_DEBUG_LOG_ERROR("[OpenGL_Shader.cpp]: shader link error: {0}", infoLog);
			return false;
		}
		return true;
	}
}


