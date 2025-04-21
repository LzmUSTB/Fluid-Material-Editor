#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/Camera/FixedFocusCamera.h"
#include "core/Macros.h"
#include "renderer/Mesh.h"

namespace FMEditor {

	enum FME_ENTITY_TYPE {
		ASSET,
		OBJECT
	};

	enum FME_RENDER_TYPE {
		FME_PARTICLE,
		FME_SKYBOX,
		FME_RIGIDBODY
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

	//struct C_Position {
	//	glm::vec3 c_Position;

	//	C_Position(float x, float y, float z)
	//		: c_Position(x, y, z) {}
	//};

	//struct C_Velocity {
	//	glm::vec3 c_Velocity;
	//	C_Velocity() :c_Velocity(0, 0, 0) {}
	//};

	//struct C_Acceleration {
	//	glm::vec3 c_Acceleration;
	//	C_Acceleration() :c_Acceleration(0, 0, 0) {}
	//};

	//struct C_ParticleStatus {
	//	// x: mass
	//	// y: density
	//	// z: pressure
	//	glm::vec3 c_ParticleStatus;
	//	C_ParticleStatus() :c_ParticleStatus(0, 0, 0) {}
	//};

	struct C_ParticleGroup {
		unsigned int c_particleCount;
		std::vector<glm::vec4>c_PositionList;
		std::vector<glm::vec4>c_VelocityList;
		std::vector<glm::vec4>c_AccelerationList;

		// x: mass
		// y: density
		// z: pressure
		// w: none
		std::vector<glm::vec4>c_StatusList;
		C_ParticleGroup(unsigned int count) :
			c_particleCount(count),
			c_PositionList(count),
			c_VelocityList(count),
			c_AccelerationList(count),
			c_StatusList(count) {}
		void SetPosition(unsigned int index, float x, float y, float z) {
			//c_PositionList.emplace(c_PositionList.begin() + index, glm::vec3(x, y, z));
			c_PositionList[index] = glm::vec4(x, y, z, 1);
		}
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
		Mesh c_Mesh;
		FME_RENDER_TYPE c_RenderType;
		//unsigned int c_ShaderID;
		//C_RenderObject(const float* vertices, unsigned int vertices_size, const unsigned int* indices, unsigned int indices_size) :
		//	c_Mesh(vertices, vertices_size, indices, indices_size) {}
		C_RenderObject(FME_RENDER_TYPE renderType, Mesh& mesh) :c_RenderType(renderType), c_Mesh(mesh) {}
	};

	struct C_BoxCollider {

	};

	struct C_Infomation {
		C_Infomation(std::string name) : c_Name(name) {}
		std::string c_Name;
	};

}