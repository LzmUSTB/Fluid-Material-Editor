#include "fmepch.h"
#include <glad/glad.h>
#include "OpenGL_Renderer.h"

namespace FMEditor {
	OpenGL_Renderer::~OpenGL_Renderer()
	{
		glDeleteFramebuffers(1, &m_Framebuffer);
		glDeleteTextures(1, &m_RenderTexture);
		for (OpenGL_Shader* shader : m_Shaders) {
			delete shader;
		}
	}

	void OpenGL_Renderer::Setup(int width, int height)
	{
		m_Width = width;
		m_Height = height;

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
		// TODO
	}

	void OpenGL_Renderer::EndScene()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Renderer::Submit()
	{
		// TODO
	}

	unsigned int OpenGL_Renderer::GetRenderTexture()
	{
		return m_RenderTexture;
	}

	void OpenGL_Renderer::LoadShader(const char* vertexPath, const char* fragmentPath)
	{

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
}

