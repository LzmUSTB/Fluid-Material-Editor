#include "fmepch.h"
#include "Camera.h"
#include "core/Macros.h"

namespace FMEditor {

	Camera::Camera(float x, float y, float z, int width, int height)
	{
		m_Width = (float)width;
		m_Height = (float)height;
		m_Position = glm::vec3(x, y, z);
		m_Zoom = 45.0;
		m_WorldUp = glm::vec3(0.f, 1.f, 0.f);
		m_Near = 0.1;
		m_Far = 100.f;
	}

	//void Camera::SetProjectionMatrix()
	//{
	//	m_Projection = glm::perspective(
	//		glm::radians(m_Zoom),
	//		m_Width / m_Height,
	//		0.1f,
	//		100.f
	//	);
	//}

	void Camera::ProcessWindowResize(float width, float height)
	{
		if (m_Width != width || m_Height != height) {
			m_Width = width;
			m_Height = height;
			m_Projection = glm::perspective(
				glm::radians(m_Zoom),
				m_Width / m_Height,
				0.1f,
				100.f
			);
		}
	}

	void Camera::ProcessMouseScroll(float yoffset)
	{
		m_Zoom -= (float)yoffset;
		if (m_Zoom < 1.0f)
			m_Zoom = 1.0f;
		if (m_Zoom > 60.0f)
			m_Zoom = 60.0f;

		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			0.1f,
			100.f
		);
	}
}


