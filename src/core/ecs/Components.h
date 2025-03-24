#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "renderer/Camera/Camera.h"
#include "core/Macros.h"

namespace FMEditor {

	struct C_Transform {
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		C_Transform() = default;
		C_Transform(const C_Transform&) = default;
		C_Transform(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	// TODO: component for particles
	struct C_Position {
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		C_Position(const glm::vec3& pos)
			: Position(pos) {}
	};

	struct C_Camera {
		Scope<Camera> Camera;
		unsigned int TargetTexture;
	};

	struct C_BoxCollider {

	};

}