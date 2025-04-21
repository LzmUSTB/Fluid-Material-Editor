#include "fmepch.h"
#include "OpenGL_FrameBuffer.h"

namespace FMEditor {
	OpenGL_FrameBuffer::OpenGL_FrameBuffer()
	{
		m_DepthBufferID = 0;
		glGenFramebuffers(1, &m_ID);

	}

	OpenGL_FrameBuffer::~OpenGL_FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
	}

	void OpenGL_FrameBuffer::Bind(bool clear)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		if (clear)
			ClearBuffer();
	}

	void OpenGL_FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_FrameBuffer::AttachTexture(uint32_t texID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
		CheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_FrameBuffer::AttachDepthTexture(uint32_t texID)
	{
		FME_DEBUG_LOG_TRACE("attach depth texture :{0}", texID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);
		m_DepthBufferID = texID;
		CheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_FrameBuffer::CreateDepthBuffer(uint32_t width, uint32_t height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glGenRenderbuffers(1, &m_DepthBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferID);
		CheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_FrameBuffer::ClearBuffer()
	{
		if (m_DepthBufferID) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	void OpenGL_FrameBuffer::CheckError() const
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			FME_LOG_ERROR("[OpenGL_FrameBuffer.cpp]: frame buffer is not complete, ID: %s", m_ID);
			FME_DEBUG_LOG_ERROR("[OpenGL_FrameBuffer.cpp]: frame buffer is not complete, ID: {0}", m_ID);
			FME_DEBUG_ASSERT(0);
		}
	}
}