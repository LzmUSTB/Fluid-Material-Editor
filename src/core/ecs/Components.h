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
		float c_Scale;
		std::vector<glm::vec4>c_PositionList;
		std::vector<glm::vec4>c_VelocityList;			// vec3 velocity + float mass
		std::vector<glm::mat3>c_DeformationGradient;
		std::vector<glm::mat3>c_AffineVelocityField;
		std::vector<float>c_Plasticity;
		std::vector<glm::vec4>c_SPH_Force;				// w unused

		C_ParticleGroup(unsigned int count, float density) :
			c_ParticleCount(count),
			c_Density(density),
			c_PositionList(count),
			c_VelocityList(count),
			c_DeformationGradient(count),
			c_AffineVelocityField(count),
			c_Plasticity(count),
			c_SPH_Force(count),
			c_Scale(1.) {}

		void SetPosition(unsigned int index, float x, float y, float z) {
			c_PositionList[index] = glm::vec4(x, y, z, 0);
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
		void SetScale(float scale) { c_Scale = scale; }
	};

	struct C_MLSMPM_Grid {
		// vec3 velocity + float mass
		std::vector<glm::vec4>c_GridStatus;
		int c_ParticleCount;
		glm::vec3 c_GridResolution;
		float c_GridSpacing;
		glm::vec3 c_GridOrigin;

		C_MLSMPM_Grid(glm::vec3 gridRes, float gridSpacing, int particleCount) :
			c_GridStatus(gridRes.x* gridRes.y* gridRes.z),
			c_GridResolution(gridRes),
			c_ParticleCount(particleCount),
			c_GridSpacing(gridSpacing),
			c_GridOrigin(-gridSpacing / 2 * gridRes) {}

		inline int GetIndex(int x, int y, int z) const {
			return x + y * c_GridResolution.x + z * c_GridResolution.y * c_GridResolution.x;
		}
	};

	struct C_SPH_Grid {
		glm::vec3 c_GridResolution;
		glm::vec3 c_GridOrigin;

		float c_CellSize;
		int c_CellCount;
		int c_ParticleCount;
		std::vector<glm::uint32_t> c_HashCount;
		std::vector<glm::uint32_t> c_HashCount2;
		std::vector<glm::uint32_t> c_OffsetArr;
		std::vector<glm::uint32_t> c_IndexArr;
		std::vector<glm::vec4> c_Force;

		C_SPH_Grid(glm::vec3 gridRes, float cellSize, int particleCount) :
			c_CellCount(c_GridResolution.x* c_GridResolution.y* c_GridResolution.z),
			c_HashCount(c_CellCount),
			c_HashCount2(c_CellCount),
			c_OffsetArr(c_CellCount),
			c_IndexArr(c_CellCount),
			c_GridResolution(gridRes),
			c_ParticleCount(particleCount),
			c_GridOrigin(-cellSize / 2 * gridRes),
			c_CellSize(cellSize) {
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