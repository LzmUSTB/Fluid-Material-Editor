#pragma once
#include <glad/glad.h>
#include "renderer/Renderer.h"
#include "OpenGL_Shader.h"

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
		void LoadShader(const char* vertexPath, const char* fragmentPath) override;
		void LoadTexture(const char* texturePath) override;
		const char* Get_API_Version() override;
		const char* Get_Device_Name() override;
	private:
		GLuint m_Framebuffer;
		GLuint m_RenderTexture;
		uint32_t m_Width;
		uint32_t m_Height;
		std::vector<OpenGL_Shader*> m_Shaders;
	};
}


