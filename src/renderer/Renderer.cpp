#include "fmepch.h"
#include "Renderer.h"
#include "platform/openGL/OpenGL_Renderer.h"

namespace FMEditor {
	Scope<Renderer> Renderer::Create()
	{
		// TODO: add OpenGL flag
		return CreateScope<OpenGL_Renderer>();
	}
}
