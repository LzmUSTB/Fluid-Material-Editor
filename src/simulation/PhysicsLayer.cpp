#include "fmepch.h"
#include "PhysicsLayer.h"
#include "core/ecs/Components.h"

namespace FMEditor {
	PhysicsLayer::PhysicsLayer(entt::registry& registry) : Layer("PhysicsLayer"), m_Registry(registry)
	{
		PI = glm::pi<float>();

		// particles
		entt::entity particles = m_Registry.create();

		int LENGTH = 50;
		int WIDTH = 50;
		int HEIGHT = 50;
		float interval = 0.02;

		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(particles, WIDTH * HEIGHT * LENGTH);
		unsigned int index = 0;
		for (int i = 0; i < LENGTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				for (int k = 0; k < WIDTH; k++) {
					float x = -float(WIDTH - 1) / 2.f * interval + i * interval;
					float y = -float(HEIGHT - 1) / 2.f * interval + j * interval;
					float z = -float(LENGTH - 1) / 2.f * interval + k * interval;
					particleGroup.SetPosition(index++, x, y, z);
				}
			}
		}

		positionSSBO = CreateSSBO(particleGroup.c_PositionList);
	}

	void PhysicsLayer::OnUpdate(float deltaTime)
	{
		BindSSBO(positionSSBO);
	}

	float PhysicsLayer::kernelDensity(float r, float h)
	{
		if (r >= h) return 0.0;
		float hSqr = h * h;
		float volume = PI * hSqr * h * 315.0 / 64.0;
		return glm::pow((hSqr - r * r), 3) / volume;
	}

	glm::vec3 PhysicsLayer::kernelSpikyGradient(const glm::vec3 rVec, float h)
	{
		float r = glm::length(glm::normalize(rVec));
		if (r >= h || r == 0.0) return glm::vec3(0, 0, 0);
		float h6 = glm::pow(h, 6);
		float factor = -45.0 / (PI * h6) * glm::pow(h - r, 2) / r;
		return factor * rVec;
	}

	float PhysicsLayer::kernelViscosityLaplacian(float r, float h)
	{
		if (r >= h) return 0.0;
		return 45.0 / (PI * glm::pow(h, 6)) * (h - r);
	}


}

