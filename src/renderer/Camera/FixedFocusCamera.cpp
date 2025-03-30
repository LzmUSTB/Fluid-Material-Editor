#include "fmepch.h"
#include "FixedFocusCamera.h"
#include "core/Macros.h"

namespace FMEditor {
	FixedFocusCamera::FixedFocusCamera(float x, float y, float z, int width, int height) :Camera(x, y, z, width, height)
	{
		m_Focus = glm::vec3(0, 0, 0);
		m_Front = m_Focus - m_Position;
		UpdateCameraStatus();
	}

	void FixedFocusCamera::ProcessMouseDrag(float xOffset, float yOffset)
	{
		// TODO: wrong algorithm 
		float hScrollDegree = glm::radians(-180.f * xOffset / m_Width);
		float vScrollDegree = glm::radians(-180.f * yOffset / m_Height);
		m_Front.x = m_Front.x * glm::cos(hScrollDegree) + m_Front.z * glm::sin(hScrollDegree);
		m_Front.z = -m_Front.x * glm::sin(hScrollDegree) + m_Front.z * glm::cos(hScrollDegree);
		float frontZ = m_Front.y * glm::sin(vScrollDegree) + m_Front.z * glm::cos(vScrollDegree);
		if (frontZ * m_Front.z > 0) {
			m_Front.y = m_Front.y * glm::cos(vScrollDegree) - m_Front.z * glm::sin(vScrollDegree);
			m_Front.z = m_Front.y * glm::sin(vScrollDegree) + m_Front.z * glm::cos(vScrollDegree);
		}
		//glm::mat4 rotMat_h = glm::rotate(glm::mat4(1.0f), hScrollDegree, glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::mat4 rotMat_v = glm::rotate(glm::mat4(1.0f), vScrollDegree, glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::vec3 m_Front = glm::vec3(rotMat_h * rotMat_v * glm::vec4(m_Front, 1.0f));
		m_Position = m_Focus - m_Front;
		UpdateViewMatrix();
	}

	void FixedFocusCamera::UpdateCameraStatus()
	{
		glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, m_Front));
		m_View = glm::lookAt(m_Position, m_Position + m_Front, up);
		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			0.1f,
			100.f
		);
	}

	void FixedFocusCamera::UpdateProjectionMatrix()
	{
		m_Projection = glm::perspective(
			glm::radians(m_Zoom),
			m_Width / m_Height,
			0.1f,
			100.f
		);
	}

	void FixedFocusCamera::UpdateViewMatrix()
	{
		glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, m_Front));
		m_View = glm::lookAt(m_Position, m_Position + m_Front, up);
	}
}

