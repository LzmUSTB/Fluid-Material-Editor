#include "fmepch.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace FMEditor {

	Camera::Camera(glm::vec3 position, glm::vec3 direction)
	{
		m_Position = position;
		m_Direction = direction;
		m_WorldUp = glm::vec3(0.f, 1.f, 0.f);
	}

	void Camera::SetProjectionMatrix()
	{

	}

	void Camera::UpdateCameraStatus()
	{
		glm::vec3 front{};
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
		m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void Camera::ProcessMouseScroll(float yoffset)
	{
		m_Zoom -= (float)yoffset;
		if (m_Zoom < 1.0f)
			m_Zoom = 1.0f;
		if (m_Zoom > 45.0f)
			m_Zoom = 45.0f;
	}
}


