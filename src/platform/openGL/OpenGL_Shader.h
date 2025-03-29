#pragma once
#include <glad/glad.h>
#include "renderer/Shader.h"
#include <glm/glm.hpp>

namespace FMEditor {

	class OpenGL_Shader :public Shader
	{
	public:
		GLuint m_ID;

		OpenGL_Shader(const char* vertexPath, const char* fragmentPath);
		~OpenGL_Shader();

		void Bind() override;
		void Unbind() override;
		void setMat4(const char* name, const glm::mat4& mat) const override;
		void setVec3(const char* name, float x, float y, float z) const override;
		void setVec4(const char* name, float x, float y, float z, float w) const override;
	private:
		bool CheckShaderError(GLint shaderID);
		bool CheckProgramError(GLint programID, GLint vertexID, GLint fragID);
	};

}
