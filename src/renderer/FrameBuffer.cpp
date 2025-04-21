#include "fmepch.h"
#include "FrameBuffer.h"
#include "platform/openGL/OpenGL_FrameBuffer.h"

namespace FMEditor {
	Scope<FrameBuffer> FrameBuffer::Create()
	{
		// TODO: add OpenGL flag
		return CreateScope<OpenGL_FrameBuffer>();
	}
}