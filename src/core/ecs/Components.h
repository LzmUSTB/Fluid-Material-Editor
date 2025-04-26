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

	struct C_ParticleGroup {
		unsigned int c_ParticleCount;
		float c_Density;
		std::vector<glm::vec4>c_PositionList;
		std::vector<glm::vec4>c_VelocityList;			// vec3 velocity + float mass
		std::vector<glm::mat3>c_DeformationGradient;
		std::vector<glm::mat3>c_AffineVelocityField;
		std::vector<float>c_Plasticity;

		C_ParticleGroup(unsigned int count, float density) :
			c_ParticleCount(count),
			c_Density(density),
			c_PositionList(count),
			c_VelocityList(count),
			c_DeformationGradient(count),
			c_AffineVelocityField(count),
			c_Plasticity(count) {
		}
		void SetPosition(unsigned int index, float x, float y, float z) {
			c_PositionList[index] = glm::vec4(x, y, z, 1);
		}
		void SetVelocityAndMass(unsigned int index, float x, float y, float z, float mass) {
			c_VelocityList[index] = glm::vec4(x, y, z, mass);
		}
		void SetDeformationGradient(unsigned int index, glm::mat3 mat) {
			c_DeformationGradient[index] = mat;
		}
		void SetAffineVelocityField(unsigned int index, glm::mat3 mat) {
			c_AffineVelocityField[index] = mat;
		}
		void SetPlasticity(unsigned int index, float plasticity) {
			c_Plasticity[index] = plasticity;
		}
	};

	struct C_Grid {
		// vec3 velocity + float mass
		std::vector<glm::vec4>c_GridStatus;

		int c_GridResolution;
		float c_GridSpacing;
		glm::vec3 c_GridOrigin;

		C_Grid(int gridRes, float gridSpacing, int particleCount) :
			c_GridStatus(gridRes* gridRes* gridRes),
			c_GridResolution(gridRes),
			c_GridSpacing(gridSpacing),
			c_GridOrigin(-gridSpacing* gridRes / 2) {}

		inline int GetIndex(int x, int y, int z) const {
			return x + y * c_GridResolution + z * c_GridResolution * c_GridResolution;
		}
	};

	struct C_Camera {
		C_Camera(float x, float y, float z, int width, int height) :c_Camera(x, y, z, width, height) {
		}
		FixedFocusCamera c_Camera;
	};

	struct C_RenderObject {
		Mesh c_Mesh;
		FME_RENDER_TYPE c_RenderType;
		C_RenderObject(FME_RENDER_TYPE renderType, Mesh& mesh) :c_RenderType(renderType), c_Mesh(mesh) {}
	};

	struct C_BoxCollider {

	};

	struct C_Infomation {
		C_Infomation(std::string name) : c_Name(name) {}
		std::string c_Name;
	};

}