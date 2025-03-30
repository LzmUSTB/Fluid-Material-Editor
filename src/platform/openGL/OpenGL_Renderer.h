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
		void BindShader(unsigned int shaderID) override;
		unsigned int GetRenderTexture() override;
		//void LoadTexture(const char* texturePath) override;
		const char* Get_API_Version() override;
		const char* Get_Device_Name() override;
		void DrawMesh(Mesh& mesh) override;
		void SetupMesh(Mesh& mesh) override;
		void EnableDepthMask(bool enable) override;
		unsigned int LoadCubeMap(std::vector<std::string> pathList) override;
		void BindCubeMap(unsigned int textureID) override;

	private:
		GLuint m_Framebuffer;
		GLuint m_RenderTexture;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_ShaderIndex = 0;
	};
}


