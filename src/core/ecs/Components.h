#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/Camera/FixedFocusCamera.h"
#include "core/Macros.h"
#include "renderer/Mesh.h"

namespace FMEditor {

	enum ENTITY_TYPE {
		SHADER,
		OBJECT
	};
	//struct C_Transform {
	//	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	//	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	//	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	//	C_Transform() = default;
	//	C_Transform(const C_Transform&) = default;
	//	C_Transform(const glm::vec3& translation)
	//		: Translation(translation) {}

	//	glm::mat4 GetTransform() const
	//	{
	//		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

	//		return glm::translate(glm::mat4(1.0f), Translation)
	//			* rotation
	//			* glm::scale(glm::mat4(1.0f), Scale);
	//	}
	//};

	// TODO: component for particles
	struct C_Position {
		glm::vec3 c_Position = { 0.0f, 0.0f, 0.0f };
		C_Position(const glm::vec3& pos)
			: c_Position(pos) {}
	};

	struct C_Camera {
		C_Camera(float x, float y, float z, int width, int height) :c_Camera(x, y, z, width, height) {
		}
		FixedFocusCamera c_Camera;
		//glm::mat4 c_ViewMatrix;
		//glm::mat4 c_ProjectionMatrix;
		//float c_Zoom;

		//unsigned int c_TargetTexture;
	};

	struct C_RenderObject {
		Mesh c_mesh;
		//unsigned int c_ShaderID;
		//C_RenderObject(const float* vertices, unsigned int vertices_size, const unsigned int* indices, unsigned int indices_size) :
		//	c_mesh(vertices, vertices_size, indices, indices_size) {}
		C_RenderObject(Mesh& mesh) :c_mesh(mesh) {}
	};

	struct C_BoxCollider {

	};

	struct C_Infomation {
		C_Infomation(std::string name) : c_Name(name) {}
		std::string c_Name;
	};

}