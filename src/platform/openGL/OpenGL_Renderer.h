#pragma once
#include <glad/glad.h>
#include "renderer/Renderer.h"

namespace FMEditor {
	class OpenGL_Renderer : public Renderer
	{
	public:
		~OpenGL_Renderer();
		void Setup(int width, int height) override;
		void BeginScene() override;
		void EndScene() override;
		void Submit() override;
		unsigned int GetRenderTexture() override;

	private:
		GLuint m_Framebuffer;
		GLuint m_Texture;
		uint32_t m_Width;
		uint32_t m_Height;
	};
}


