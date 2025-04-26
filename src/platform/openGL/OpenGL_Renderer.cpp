#include "fmepch.h"
#include <glad/glad.h>
#include "OpenGL_Renderer.h"
#include "core/stb_image.h"

namespace FMEditor {
	OpenGL_Renderer::OpenGL_Renderer()
	{
		float vertices[] = {
			1.f, 1.f, 0.0f,
			1.f, -1.f, 0.0f,
			-1.f, -1.f, 0.0f,
			-1.f, 1.f, 0.0f
		};
		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};
		m_ScreenQuad = CreateScope<Mesh>(vertices, sizeof(vertices), indices, sizeof(indices));
		SetupMesh(*m_ScreenQuad);
	}

	OpenGL_Renderer::~OpenGL_Renderer()
	{

	}

	void OpenGL_Renderer::Setup(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		//openGL init
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glClearColor(0, 0, 0, 1);
		glViewport(0, 0, m_Width, m_Height);
	}

	void OpenGL_Renderer::BeginScene()
	{
		// TODO: temp
	}

	void OpenGL_Renderer::EndScene()
	{
		// TODO: temp
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Renderer::BeginParticleRender()
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	void OpenGL_Renderer::EndParticleRender()
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGL_Renderer::BindShader(uint32_t shaderID)
	{
		glUseProgram(shaderID);
	}

	void OpenGL_Renderer::BindFramebuffer(uint32_t frameBufferID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	}

	const char* OpenGL_Renderer::Get_API_Version()
	{
		return (const char*)glGetString(GL_VERSION);
	}

	const char* OpenGL_Renderer::Get_Device_Name()
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	int OpenGL_Renderer::Get_MaximumSSBO()
	{
		int maxSSBOs = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOs);
		return maxSSBOs;
	}

	void OpenGL_Renderer::DrawMesh(Mesh& mesh)
	{
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void OpenGL_Renderer::DrawMeshInstanced(Mesh& mesh, uint32_t count)
	{
		glBindVertexArray(mesh.VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.indices.size(), count);
		glBindVertexArray(0);
	}

	void OpenGL_Renderer::DrawPoints(float pointSize, uint32_t count)
	{
		static bool once = true;
		static uint32_t  temp;
		if (once) {
			glGenVertexArrays(1, &temp);
			once = false;
		}
		glBindVertexArray(temp);
		glPointSize(pointSize);
		glDrawArraysInstanced(GL_POINTS, 0, 1, count);
	}

	void OpenGL_Renderer::DrawQuad()
	{
		glBindVertexArray(m_ScreenQuad->VAO);
		glDrawElements(GL_TRIANGLES, m_ScreenQuad->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void OpenGL_Renderer::EnableBlend(bool enable)
	{
		if (enable) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	void OpenGL_Renderer::SetupMesh(Mesh& mesh)
	{
		// create buffers/arrays
		glGenVertexArrays(1, &mesh.VAO);
		glGenBuffers(1, &mesh.VBO);
		glGenBuffers(1, &mesh.EBO);

		glBindVertexArray(mesh.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &mesh.vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// vertex normals
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		//glEnableVertexAttribArray(3);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		//glEnableVertexAttribArray(4);
		//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		//glEnableVertexAttribArray(5);
		//glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		// weights
		//glEnableVertexAttribArray(6);
		//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

		glBindVertexArray(0);
	}

	void OpenGL_Renderer::EnableDepthMask(bool enable)
	{
		glDepthMask(enable ? GL_TRUE : GL_FALSE);
	}

	unsigned int OpenGL_Renderer::LoadCubeMap(std::vector<std::string> pathList)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < pathList.size(); i++) {
			unsigned char* data = stbi_load(pathList[i].c_str(), &width, &height, &nrChannels, 3);
			if (data) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else {
				FME_DEBUG_LOG_ERROR("Failed to load cubemap texture: {0}", pathList[i]);
				FME_LOG_ERROR("Failed to load cubemap texture: %s", pathList[i]);
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	void OpenGL_Renderer::BindCubeMap(uint32_t textureID)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

}

