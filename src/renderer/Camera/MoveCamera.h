#pragma once
#include "Camera.h"

namespace FMEditor {
	class MoveCamera : public Camera
	{
	public:
		MoveCamera(float x, float y, float z, int width, int height);


	private:
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;
		float m_Yaw;
		float m_Pitch;

		// Inherited via Camera
		void UpdateCameraStatus() override;
		void UpdateProjectionMatrix() override;
		void UpdateViewMatrix() override;
	};
}


