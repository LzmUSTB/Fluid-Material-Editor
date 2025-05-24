#include "fmepch.h"
#include "FixedFocusCamera.h"
#include "core/Macros.h"

namespace FMEditor {
	FixedFocusCamera::FixedFocusCamera(float x, float y, float z, int width, int height) :Camera(x, y, z, width, height)
	{
		m_Focus = glm::vec3(0, 0, 0);
		m_Front = m_Focus - m_Position;
		m_Distance = glm::length(m_Front);
		m_Front = glm::normalize(m_Front);
		m_Yaw = atan2(m_Front.x, m_Front.z);
		m_Pitch = asin(m_Front.y);
		UpdateCameraStatus();
	}

	void FixedFocusCamera::ProcessMouseDrag(float xOffset, float yOffset)
	{
		m_Yaw += glm::radians(-180.f * xOffset / m_Width);
		m_Pitch += glm::radians(-180.f * yOffset / m_Height);

		constexpr float pitchLimit = glm::radians(89.0f);
		m_Pitch = glm::clamp(m_Pitch, -pitchLimit, pitchLimit);

		glm::vec3 newFront(0);
		newFront.x = cos(m_Pitch) * sin(m_Yaw);
		newFront.y = sin(m_Pitch);
		newFront.z = cos(m_Pitch) * cos(m_Yaw);
		m_Front = glm::normalize(newFront);

		m_Position = m_Focus - m_Front * m_Distance;

		UpdateViewMatrix();
		//float hScrollDegree = glm::radians(-180.f * xOffset / m_Width);
		//float vScrollDegree = glm::radians(-180.f * yOffset / m_Height);
		//m_Front.x = m_Front.x * glm::cos(hScrollDegree) + m_Front.z * glm::sin(hScrollDegree);
		//m_Front.z = -m_Front.x * glm::sin(hScrollDegree) + m_Front.z * glm::cos(hScrollDegree);
		//float frontZ = m_Front.y * glm::sin(vScrollDegree) + m_Front.z * glm::cos(vScrollDegree);
		//if (frontZ * m_Front.z > 0) {
		//	m_Front.y = m_Front.y * glm::cos(vScrollDegree) - m_Front.z * glm::sin(vScrollDegree);
		//	m_Front.z = m_Front.y * glm::sin(vScrollDegree) + m_Front.z * glm::cos(vScrollDegree);
		//}

		//m_Position = m_Focus - m_Front;
		//UpdateViewMatrix();
	}

	void FixedFocusCamera::UpdateCameraStatus()
	{
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	void FixedFocusCamera::UpdateProjectionMatrix()
	{
		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			m_Near,
			m_Far
		);
	}

	void FixedFocusCamera::UpdateViewMatrix()
	{
		glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, m_Front));
		m_View = glm::lookAt(m_Position, m_Position + m_Front, up);
	}
}

