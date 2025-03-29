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
		void Submit(glm::mat4 view, glm::mat4 projection) override;
		unsigned int GetRenderTexture() override;
		void LoadShader(const char* vertexPath, const char* fragmentPath) override;
		void LoadTexture(const char* texturePath) override;
		const char* Get_API_Version() override;
		const char* Get_Device_Name() override;
		void DrawMesh(Mesh& mesh) override;
		void SetupMesh(Mesh& mesh) override;
		void SetModelMatrix(glm::mat4 model) override;
	private:
		OpenGL_Shader* m_Shader;
		GLuint m_Framebuffer;
		GLuint m_RenderTexture;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_ShaderIndex = 0;
		//std::vector<OpenGL_Shader*> m_Shaders;
	};
}


