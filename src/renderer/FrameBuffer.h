#pragma once
#include "core/Macros.h"

namespace FMEditor {
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;
		virtual void Bind(bool clear = true) = 0;
		virtual void Unbind() = 0;
		virtual void AttachTexture(uint32_t texID) = 0;
		virtual void AttachDepthTexture(uint32_t texID) = 0;
		virtual void CreateDepthBuffer(uint32_t width, uint32_t height) = 0;
		virtual void ClearBuffer() = 0;
		static Scope<FrameBuffer> Create();
	};
}


