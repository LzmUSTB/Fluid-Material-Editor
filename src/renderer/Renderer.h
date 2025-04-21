#pragma once
#include "core/Macros.h"
#include <glm/glm.hpp>
#include "Mesh.h"

namespace FMEditor {

	class Renderer
	{
	public:
		virtual ~Renderer() = default;
		virtual void Setup(uint32_t width, uint32_t height) = 0;
		virtual void SetupMesh(Mesh& mesh) = 0;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;

		virtual void BeginParticleRender() = 0;
		virtual void EndParticleRender() = 0;

		virtual void BindShader(uint32_t shaderID) = 0;
		virtual void BindCubeMap(uint32_t textureID) = 0;
		virtual void BindFramebuffer(uint32_t frameBufferID) = 0;

		//virtual void Submit(glm::mat4 view, glm::mat4 projection) = 0;
		//virtual void Submit(unsigned int shaderID, glm::mat4 view, glm::mat4 projection) = 0;
		//virtual void LoadShader(const char* vertexPath, const char* fragmentPath) = 0;
		//virtual void LoadTexture(const char* texturePath) = 0;
		virtual uint32_t LoadCubeMap(std::vector<std::string> pathList) = 0;

		virtual const char* Get_API_Version() = 0;
		virtual const char* Get_Device_Name() = 0;

		virtual void DrawMesh(Mesh& mesh) = 0;
		virtual void DrawMeshInstanced(Mesh& mesh, uint32_t count) = 0;
		virtual void DrawPoints(float pointSize, uint32_t count) = 0;
		virtual void DrawQuad() = 0;

		//virtual void SetModelMatrix(glm::mat4 model) = 0;
		virtual void EnableDepthMask(bool enable) = 0;
		virtual void EnableBlend(bool enable) = 0;
		static Scope<Renderer> Create();
	};

}


