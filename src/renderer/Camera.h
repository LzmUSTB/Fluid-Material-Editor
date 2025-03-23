#pragma once
#include "glm/glm.hpp"

namespace FMEditor {

	class Camera
	{
	public:
		// TODO
	protected:
		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::vec3 m_Position;
		glm::vec3 m_Direction;
	};

}


