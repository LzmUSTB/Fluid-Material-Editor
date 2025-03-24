#pragma once
#include "glm/glm.hpp"

namespace FMEditor {

	class Camera
	{
	public:
		Camera(glm::vec3 position, glm::vec3 direction);
		virtual void SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) = 0;
		void SetProjectionMatrix();

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetDirection() const { return m_Direction; }
		glm::mat4 GetViewMatrix() const { return m_View; }
		glm::mat4 GetProjectionMatrix() const { return m_Projection; }

	protected:
		void UpdateCameraStatus();
		void ProcessMouseScroll(float yoffset);
		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		glm::vec3 m_WorldUp;
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;
		glm::mat4 m_View;
		glm::mat4 m_Projection;
		float m_Yaw;
		float m_Pitch;
		float m_MovementSpeed;
		float m_MouseSensitivity;
		float m_Zoom;
	};

}


