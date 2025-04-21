#pragma once
#include <glad/glad.h>
#include "renderer/Renderer.h"
#include "OpenGL_Shader.h"
#include "core/Macros.h"

namespace FMEditor {
	class OpenGL_Renderer : public Renderer
	{
	public:
		OpenGL_Renderer();
		~OpenGL_Renderer();
		void Setup(uint32_t width, uint32_t height) override;
		void SetupMesh(Mesh& mesh) override;

		void BeginScene() override;
		void EndScene() override;

		void BeginParticleRender() override;
		void EndParticleRender() override;

		void BindShader(uint32_t shaderID) override;
		void BindFramebuffer(uint32_t frameBufferID) override;
		void BindCubeMap(uint32_t textureID) override;

		const char* Get_API_Version() override;
		const char* Get_Device_Name() override;

		void DrawMesh(Mesh& mesh) override;
		void DrawMeshInstanced(Mesh& mesh, uint32_t count) override;
		void DrawPoints(float pointSize, uint32_t count) override;
		void DrawQuad() override;

		void EnableDepthMask(bool enable) override;
		void EnableBlend(bool enable) override;;

		uint32_t LoadCubeMap(std::vector<std::string> pathList) override;
		//void LoadTexture(const char* texturePath) override;
	private:
		Scope<Mesh> m_ScreenQuad;
		uint32_t m_Width;
		uint32_t m_Height;
	};
}


