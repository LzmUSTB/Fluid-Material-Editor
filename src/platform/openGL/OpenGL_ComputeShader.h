#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
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
		void Dispatch(int size_x, int size_y, int size_z);
		void setFloat(const char* name, float f);
		void setVec4(const char* name, glm::vec3 origin, float res);
		void setVec3(const char* name, glm::vec3 vec);
		void setInt(const char* name, int i);
		void setIVec3(const char* name, glm::ivec3 vec);
	private:
		bool CheckShaderError(GLint shaderID);
		bool CheckProgramError(GLint programID, GLint compID);
	};

}

