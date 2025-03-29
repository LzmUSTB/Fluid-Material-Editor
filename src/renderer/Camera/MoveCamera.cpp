#include "fmepch.h"
#include "MoveCamera.h"

namespace FMEditor {
	MoveCamera::MoveCamera(float x, float y, float z, int width, int height) :Camera(x, y, z, width, height)
	{
		m_Yaw = -90.f;
		m_Pitch = 0.f;
		UpdateCameraStatus();
	}

	void MoveCamera::UpdateCameraStatus()
	{
		glm::vec3 front{};
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
		m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			0.1f,
			100.f
		);
	}
	void MoveCamera::UpdateProjectionMatrix()
	{
		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			0.1f,
			100.f
		);
	}
	void MoveCamera::UpdateViewMatrix()
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
}

