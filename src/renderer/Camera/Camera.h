#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace FMEditor {

	class Camera
	{
	public:
		Camera(float x, float y, float z, int width, int height);

		glm::vec3 GetPosition() const { return m_Position; }
		glm::mat4 GetViewMatrix() const { return m_View; }
		glm::mat4 GetProjectionMatrix() const { return m_Projection; }
		float GetNear() const { return m_Near; }
		float GetFar() const { return m_Far; }
		float GetFovRadians() const { return glm::radians(m_Zoom); }
		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }
		float GetAspectRatio() const { return m_Width / m_Height; }
		void ProcessWindowResize(float width, float height);
		void ProcessMouseScroll(float yoffset);

	protected:
		virtual void UpdateCameraStatus() = 0;
		virtual void UpdateProjectionMatrix() = 0;
		virtual void UpdateViewMatrix() = 0;
		glm::vec3 m_Position;
		glm::vec3 m_WorldUp;
		glm::mat4 m_View;
		glm::mat4 m_Projection;
		//float m_MovementSpeed;
		//float m_MouseSensitivity;
		float m_Zoom;
		float m_Width;
		float m_Height;
		float m_Near;
		float m_Far;
	};

}


