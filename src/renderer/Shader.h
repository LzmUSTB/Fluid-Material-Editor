#pragma once
#include "core/Macros.h"
#include "glm/glm.hpp"

namespace FMEditor {

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void setMat4(const char* name, const glm::mat4& mat) const = 0;
		virtual void setVec3(const char* name, float x, float y, float z) const = 0;
		virtual void setVec4(const char* name, float x, float y, float z, float w) const = 0;
		virtual void setInt(const char* name, int i) = 0;
		virtual void setFloat(const char* name, float f) = 0;
		virtual void setBool(const char* name, bool b) = 0;

		static Scope<Shader> Create(const char* vertexPath, const char* fragmentPath);
	};

}
