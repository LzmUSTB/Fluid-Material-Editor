#pragma once
#include <glad/glad.h>
#include "renderer/FrameBuffer.h"

namespace FMEditor {
	class OpenGL_FrameBuffer : public FrameBuffer
	{
	public:
		OpenGL_FrameBuffer();
		~OpenGL_FrameBuffer();
		void Bind(bool clear=true) override;
		void Unbind() override;
		void AttachTexture(uint32_t texID) override;
		void AttachDepthTexture(uint32_t texID) override;
		void CreateDepthBuffer(uint32_t width, uint32_t height) override;
		void ClearBuffer() override;
	private:
		void CheckError() const;
		GLuint m_ID;
		GLuint m_DepthBufferID;
	};
}


