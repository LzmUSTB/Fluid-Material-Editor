#include "fmepch.h"
#include "PhysicsLayer.h"
#include "core/ecs/Components.h"

namespace FMEditor {
	PhysicsLayer::PhysicsLayer(entt::registry& registry) :
		Layer("PhysicsLayer"), m_Registry(registry),
		m_Paused(true), 
		m_TimeScale(1.f), 
		m_GridBoundary(30), 
		m_Stiffness(10.f), 
		m_RestDensity(0.45f), 
		m_Viscosity(0.1f)
	{
		PI = glm::pi<float>();

		// particles
		m_ParticleEntity = m_Registry.create();

		int LENGTH = 64;
		int WIDTH = 64;
		int HEIGHT = 64;
		float interval = 0.025;
		int particleCount = WIDTH * HEIGHT * LENGTH;
		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(m_ParticleEntity, particleCount, 1000.f);
		unsigned int index = 0;
		for (int i = 0; i < LENGTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				for (int k = 0; k < WIDTH; k++) {
					float x = -float(WIDTH - 1) / 2.f * interval + i * interval;
					float y = -float(HEIGHT - 1) / 2.f * interval + j * interval;
					float z = -float(LENGTH - 1) / 2.f * interval + k * interval;
					if (i == 0 && j == 0 && k == 0) {
						FME_DEBUG_LOG_TRACE("particleOrigin: {0}, {1}, {2}", x, y, z);
					}
					particleGroup.SetPosition(index, x, y, z);
					particleGroup.SetVelocityAndMass(index, 0, 0, 0, 1.f);
					particleGroup.SetDeformationGradient(index, glm::mat3(1.0f));
					particleGroup.SetAffineVelocityField(index, glm::mat3(0.f));
					particleGroup.SetPlasticity(index, 0.f);
					index++;
				}
			}
		}

		m_GridEntity = m_Registry.create();
		int gridResolution = 128;
		auto& grid = m_Registry.emplace<C_Grid>(m_GridEntity, gridResolution, interval * 1.2f, particleCount);
		FME_DEBUG_LOG_TRACE("gridOrigin: {0}, {1}, {2}", grid.c_GridOrigin.x, grid.c_GridOrigin.y, grid.c_GridOrigin.z);

		m_PositionSSBO = CreateSSBO(particleGroup.c_PositionList);
		m_VelocitySSBO = CreateSSBO(particleGroup.c_VelocityList);
		m_GridSSBO = CreateSSBO(grid.c_GridStatus);
		m_AffineVelocityFieldSSBO = CreateSSBO(particleGroup.c_AffineVelocityField);
		m_DeformationGradientSSBO = CreateSSBO(particleGroup.c_DeformationGradient);

		BindSSBO(m_PositionSSBO, 0);
		BindSSBO(m_VelocitySSBO, 1);
		BindSSBO(m_GridSSBO, 2);
		BindSSBO(m_AffineVelocityFieldSSBO, 3);
		BindSSBO(m_DeformationGradientSSBO, 4);
	}

	void PhysicsLayer::OnUpdate(float deltaTime)
	{
		auto& grid = m_Registry.get<C_Grid>(m_GridEntity);
		// clear grid
		ClearSSBO(m_GridSSBO);

		// mls-mpm
		if (!m_Paused) {
			// p2g
			m_MLSMPM_P2G_Shader->Bind();
			m_MLSMPM_P2G_Shader->setInt("gridRes", grid.c_GridResolution);
			m_MLSMPM_P2G_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
			m_MLSMPM_P2G_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
			m_MLSMPM_P2G_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
			m_MLSMPM_P2G_Shader->Dispatch(64 * 64, 1, 1);
			m_MLSMPM_P2G_Shader->Unbind();

			// p2g 2
			m_MLSMPM_P2G_2_Shader->Bind();
			m_MLSMPM_P2G_2_Shader->setInt("gridRes", grid.c_GridResolution);
			m_MLSMPM_P2G_2_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
			m_MLSMPM_P2G_2_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
			m_MLSMPM_P2G_2_Shader->setFloat("stiffness", m_Stiffness);
			m_MLSMPM_P2G_2_Shader->setFloat("rest_density", m_RestDensity);
			m_MLSMPM_P2G_2_Shader->setFloat("dynamic_viscosity", m_Viscosity);
			m_MLSMPM_P2G_2_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
			m_MLSMPM_P2G_2_Shader->Dispatch(64 * 64, 1, 1);
			m_MLSMPM_P2G_2_Shader->Unbind();

			// grid update
			m_MLSMPM_SIM_Shader->Bind();
			m_MLSMPM_SIM_Shader->setInt("gridRes", grid.c_GridResolution);
			m_MLSMPM_SIM_Shader->setInt("gridBoundary", m_GridBoundary);
			m_MLSMPM_SIM_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
			m_MLSMPM_SIM_Shader->Dispatch(16, 16, 16);
			m_MLSMPM_SIM_Shader->Unbind();

			//g2p
			m_MLSMPM_G2P_Shader->Bind();
			m_MLSMPM_G2P_Shader->setInt("gridRes", grid.c_GridResolution);
			m_MLSMPM_G2P_Shader->setInt("gridBoundary", m_GridBoundary);
			m_MLSMPM_G2P_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
			m_MLSMPM_G2P_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
			m_MLSMPM_G2P_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
			m_MLSMPM_G2P_Shader->Dispatch(64 * 64, 1, 1);
			m_MLSMPM_G2P_Shader->Unbind();
		}
	}

	void PhysicsLayer::OnAttach()
	{
		LoadResources();
	}

	void PhysicsLayer::OnImguiRender()
	{
		auto& particleGroup = m_Registry.get<C_ParticleGroup>(m_ParticleEntity);
		ImGui::Begin("Simulation setting");
		if (ImGui::Button("Reset Position")) {
			FME_DEBUG_LOG_TRACE("Reset Particle Position and Velocity");
			UpdateSSBO(m_PositionSSBO, particleGroup.c_PositionList);
			UpdateSSBO(m_VelocitySSBO, particleGroup.c_VelocityList);
			UpdateSSBO(m_AffineVelocityFieldSSBO, particleGroup.c_AffineVelocityField);
		}
		ImGui::Checkbox("Paused", &m_Paused);
		ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.1f, 1.f, "%.3f");
		ImGui::SliderFloat("Stiffness", &m_Stiffness, 0.f, 20.f, "%.3f");
		ImGui::SliderFloat("Density", &m_RestDensity, 0.f, 10.f, "%.3f");
		ImGui::SliderFloat("Viscosity", &m_Viscosity, 0.f, 1.f, "%.3f");
		ImGui::SliderInt("Grid Boundary", &m_GridBoundary, 1, 40, "%d");
		ImGui::End();
	}

	void PhysicsLayer::LoadResources()
	{
		m_MLSMPM_P2G_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm_p2g.comp");
		m_MLSMPM_P2G_2_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm_p2g_2.comp");
		m_MLSMPM_SIM_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm_sim.comp");
		m_MLSMPM_G2P_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm_g2p.comp");
	}
}

