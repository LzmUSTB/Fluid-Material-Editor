#include "fmepch.h"
#include "OpenGL_Shader.h"
#include "core/Macros.h"

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
			FME_ERROR("shader file not successfully read!");
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		///////////////////
		// vertex shader //--------------------------------------
		///////////////////
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		if (!CheckShaderError(vertexShader))
			FME_ERROR("vertex shader : failed to compile");

		/////////////////////
		// fragment shader //------------------------------------
		/////////////////////
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		if (!CheckShaderError(fragmentShader))
			FME_ERROR("fragment shader : failed to compile");

		//////////
		// link //------------------------------------
		//////////
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);
		if (!CheckProgramError(m_ID, vertexShader, fragmentShader)) {
			FME_ERROR("shader : failed to link");
		}

		glDetachShader(m_ID, vertexShader);
		glDetachShader(m_ID, fragmentShader);
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

	bool OpenGL_Shader::CheckShaderError(GLint shaderID)
	{
		GLint isCompiled = 0;

		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shaderID);

			FME_ERROR("{0}", infoLog.data());
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
			GLint maxLength = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(programID);
			glDeleteShader(vertexID);
			glDeleteShader(fragID);

			FME_ERROR("{0}", infoLog.data());
			return false;
		}
		return true;
	}
}


