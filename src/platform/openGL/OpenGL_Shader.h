#pragma once
#include <glad/glad.h>
#include "renderer/Shader.h"

namespace FMEditor {

	class OpenGL_Shader :public Shader
	{
	public:
		OpenGL_Shader(const char* vertexPath, const char* fragmentPath);
		~OpenGL_Shader();
		void Bind() override;
		void Unbind() override;

	private:
		bool CheckShaderError(GLint shaderID);
		bool CheckProgramError(GLint programID, GLint vertexID, GLint fragID);
	private:
		GLuint m_ID;
	};

}
