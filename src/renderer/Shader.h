#pragma once
#include "core/Macros.h"

namespace FMEditor {

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		static Scope<Shader> Create(const char* vertexPath, const char* fragmentPath);
	};

}
