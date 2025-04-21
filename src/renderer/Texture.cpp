#include "fmepch.h"
#include "Texture.h"
#include "platform/openGL/OpenGL_Texture.h"

namespace FMEditor {

	Scope<Texture> Texture::Create(uint32_t width, uint32_t height, bool rgba, bool isFloat, bool depth)
	{
		return CreateScope<OpenGL_Texture>(width, height, rgba, isFloat, depth);
	}

}