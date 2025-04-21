#pragma once
#include <glad/glad.h>
#include "core/Macros.h"

namespace FMEditor {

	class OpenGL_ComputeShader
	{
	public:
		GLuint m_ID;

		OpenGL_ComputeShader(const char* path);
		~OpenGL_ComputeShader();

		void Bind();
		void Unbind();
	private:
		bool CheckShaderError(GLint shaderID);
		bool CheckProgramError(GLint programID, GLint compID);
	};

}

