#include "fmepch.h"
#include "Shader.h"
#include "platform/OpenGL/OpenGL_Shader.h"

namespace FMEditor {
	Scope<Shader> Shader::Create(const char* vertexPath, const char* fragmentPath)
	{
		// TODO: add OpenGL flag
		return CreateScope<OpenGL_Shader>(vertexPath, fragmentPath);
	}
}

