#include "fmepch.h"
#include <glad/glad.h>
#include "OpenGL_Renderer.h"

namespace FMEditor {
	OpenGL_Renderer::~OpenGL_Renderer()
	{
		glDeleteFramebuffers(1, &m_Framebuffer);
		glDeleteTextures(1, &m_RenderTexture);
		//for (OpenGL_Shader* shader : m_Shaders) {
		//	delete shader;
		//}
		delete m_Shader;
	}

	void OpenGL_Renderer::Setup(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		LoadShader("assets/shaders/test.vert", "assets/shaders/test.frag");

		glGenFramebuffers(1, &m_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

		glGenTextures(1, &m_RenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_RenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RenderTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			FME_LOG_ERROR("[OpenGL_Renderer.cpp]: frame buffer is not complete!");
			FME_DEBUG_LOG_ERROR("[OpenGL_Renderer.cpp]: frame buffer is not complete!");
			FME_DEBUG_ASSERT(0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Renderer::BeginScene()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		glViewport(0, 0, m_Width, m_Height);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// TODO: temp
		m_Shader->Bind();
	}

	void OpenGL_Renderer::EndScene()
	{
		// TODO: temp
		m_Shader->Unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Renderer::Submit(glm::mat4 view, glm::mat4 projection)
	{
		m_Shader->setMat4("View", view);
		m_Shader->setMat4("Projection", projection);
	}

	unsigned int OpenGL_Renderer::GetRenderTexture()
	{
		return m_RenderTexture;
	}

	void OpenGL_Renderer::LoadShader(const char* vertexPath, const char* fragmentPath)
	{
		m_Shader = new OpenGL_Shader(vertexPath, fragmentPath);
		//m_Shaders.emplace(m_Shaders.begin() + m_ShaderIndex, shader);
		//m_ShaderIndex++;
	}

	void OpenGL_Renderer::LoadTexture(const char* cubeMapPath)
	{

	}

	const char* OpenGL_Renderer::Get_API_Version()
	{
		return (const char*)glGetString(GL_VERSION);
	}
	const char* OpenGL_Renderer::Get_Device_Name()
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	void OpenGL_Renderer::DrawMesh(Mesh& mesh)
	{
		// draw mesh
		GL_CALL(glBindVertexArray(mesh.VAO));
		GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		glBindVertexArray(0);
		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
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
	void OpenGL_Renderer::SetModelMatrix(glm::mat4 model)
	{
		m_Shader->setMat4("Model", model);
	}
}

