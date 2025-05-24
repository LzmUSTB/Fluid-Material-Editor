#pragma once
#include "Camera.h"
namespace FMEditor {

	class FixedFocusCamera : public Camera
	{
	public:
		FixedFocusCamera(float x, float y, float z, int width, int height);
		void ProcessMouseDrag(float xOffset, float yOffset);
	private:
		glm::vec3 m_Focus;
		glm::vec3 m_Front;
		float m_Distance;
		float m_Yaw;
		float m_Pitch;

		void UpdateCameraStatus() override;
		void UpdateProjectionMatrix() override;
		void UpdateViewMatrix() override;
	};

}

