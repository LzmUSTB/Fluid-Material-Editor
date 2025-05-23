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
		m_RestDensity(0.25f),
		m_Viscosity(0.1f)
	{
		PI = glm::pi<float>();

		// particles
		m_ParticleEntity = m_Registry.create();

		int LENGTH = 64;
		int WIDTH = 64;
		int HEIGHT = 64;
		float interval = 0.025;
		float initOffset = 0;
		int particleCount = WIDTH * HEIGHT * LENGTH;
		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(m_ParticleEntity, particleCount, 1000.f);
		unsigned int index = 0;

		for (int i = 0; i < LENGTH / 2; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				for (int k = 0; k < WIDTH; k++) {
					glm::vec3 offset(initOffset + LENGTH / 2 * interval, 0, 0);

					float x = offset.x - float(WIDTH - 1) / 2.f * interval + i * interval;
					float y = offset.y - float(HEIGHT - 1) / 2.f * interval + j * interval;
					float z = offset.z - float(LENGTH - 1) / 2.f * interval + k * interval;
					if (i == 0 && j == 0 && k == 0) {
						FME_DEBUG_LOG_TRACE("particleOrigin1: {0}, {1}, {2}", x, y, z);
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

		for (int i = 0; i < LENGTH / 2; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				for (int k = 0; k < WIDTH; k++) {
					glm::vec3 offset(-initOffset, 0, 0);
					float x = offset.x - float(WIDTH - 1) / 2.f * interval + i * interval;
					float y = offset.y - float(HEIGHT - 1) / 2.f * interval + j * interval;
					float z = offset.z - float(LENGTH - 1) / 2.f * interval + k * interval;
					if (i == 0 && j == 0 && k == 0) {
						FME_DEBUG_LOG_TRACE("particleOrigin2: {0}, {1}, {2}", x, y, z);
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
		glm::vec3 gridResolution = glm::vec3(192, 128, 128);
		auto& grid = m_Registry.emplace<C_MLSMPM_Grid>(m_GridEntity, gridResolution, interval * 1.2f, particleCount);

		glm::vec3 SPHgridRange = glm::vec3(5.76, 5.76, 5.76);
		float SPHcellSize = 3.f * interval;
		glm::vec3 SPHgridResolution = glm::floor(SPHgridRange / SPHcellSize) + glm::vec3(1);
		auto& grid_sph = m_Registry.emplace<C_SPH_Grid>(m_GridEntity, SPHgridResolution, SPHcellSize, particleCount);

		FME_DEBUG_LOG_TRACE("mlsmpm gridOrigin: {0}, {1}, {2}", grid.c_GridOrigin.x, grid.c_GridOrigin.y, grid.c_GridOrigin.z);

		FME_DEBUG_LOG_TRACE("sph gridOrigin: {0}, {1}, {2}", grid_sph.c_GridOrigin.x, grid_sph.c_GridOrigin.y, grid_sph.c_GridOrigin.z);

		FME_DEBUG_LOG_TRACE("sph gridResolution: {0}, {1}, {2}", SPHgridResolution.x, SPHgridResolution.y, SPHgridResolution.z);

		// mls-mpm
		m_PositionSSBO = CreateSSBO(particleGroup.c_PositionList);
		m_VelocitySSBO = CreateSSBO(particleGroup.c_VelocityList);
		m_GridSSBO = CreateSSBO(grid.c_GridStatus);
		m_AffineVelocityFieldSSBO = CreateSSBO(particleGroup.c_AffineVelocityField);
		m_DeformationGradientSSBO = CreateSSBO(particleGroup.c_DeformationGradient);

		// sph
		m_SPH_HashCountSSBO = CreateSSBO(grid_sph.c_HashCount);
		m_SPH_HashCount_2_SSBO = CreateSSBO(grid_sph.c_HashCount2);
		m_SPH_IndexSSBO = CreateSSBO(grid_sph.c_IndexArr);
		m_SPH_OffsetSSBO = CreateSSBO(grid_sph.c_OffsetArr);
		m_SPH_ForceSSBO = CreateSSBO(grid_sph.c_Force);

		BindSSBO(m_PositionSSBO, 0);
		BindSSBO(m_VelocitySSBO, 1);
		BindSSBO(m_GridSSBO, 2);
		BindSSBO(m_AffineVelocityFieldSSBO, 3);
		BindSSBO(m_DeformationGradientSSBO, 4);

		BindSSBO(m_SPH_HashCountSSBO, 5);
		BindSSBO(m_SPH_HashCount_2_SSBO, 6);
		BindSSBO(m_SPH_IndexSSBO, 7);
		BindSSBO(m_SPH_OffsetSSBO, 8);
		BindSSBO(m_DeformationGradientSSBO, 9);
	}

	void PhysicsLayer::OnUpdate(float deltaTime)
	{
		if (!m_Paused) {
			MlsmpmMethod(deltaTime);
			//SphMethod(deltaTime);
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
		ImGui::SliderFloat("Stiffness", &m_Stiffness, 0.f, 1000.f, "%.3f");
		ImGui::SliderFloat("Density", &m_RestDensity, 0.15f, 1.f, "%.3f");
		ImGui::SliderFloat("Viscosity", &m_Viscosity, 0.f, 1000.f, "%.3f");
		ImGui::SliderInt("Grid Boundary", &m_GridBoundary, 1, 40, "%d");
		ImGui::End();


	}

	void PhysicsLayer::LoadResources()
	{
		m_MLSMPM_P2G_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm/mlsmpm_p2g.comp");
		m_MLSMPM_P2G_2_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm/mlsmpm_p2g_2.comp");
		m_MLSMPM_SIM_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm/mlsmpm_sim.comp");
		m_MLSMPM_G2P_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/mlsmpm/mlsmpm_g2p.comp");

		m_SPH_Grid1_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/sph/sph_grid1.comp");
		m_SPH_Grid2_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/sph/sph_grid2.comp");
		m_SPH_Density_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/sph/sph_density.comp");
		m_SPH_Force_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/sph/sph_force.comp");
		m_SPH_Integrate_Shader = CreateScope<OpenGL_ComputeShader>("assets/shaders/sph/sph_integrate.comp");
	}

	void PhysicsLayer::MlsmpmMethod(float deltaTime)
	{
		auto& grid = m_Registry.get<C_MLSMPM_Grid>(m_GridEntity);
		// clear grid
		ClearSSBO(m_GridSSBO);

		// p2g
		m_MLSMPM_P2G_Shader->Bind();
		m_MLSMPM_P2G_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_MLSMPM_P2G_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_MLSMPM_P2G_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
		m_MLSMPM_P2G_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_MLSMPM_P2G_Shader->Dispatch(64 * 64, 1, 1);
		m_MLSMPM_P2G_Shader->Unbind();

		// p2g 2
		m_MLSMPM_P2G_2_Shader->Bind();
		m_MLSMPM_P2G_2_Shader->setIVec3("gridRes", grid.c_GridResolution);
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
		m_MLSMPM_SIM_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_MLSMPM_SIM_Shader->setInt("gridBoundary", m_GridBoundary);
		m_MLSMPM_SIM_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_MLSMPM_SIM_Shader->Dispatch(grid.c_GridResolution.x / 8, grid.c_GridResolution.y / 8, grid.c_GridResolution.z / 8);
		m_MLSMPM_SIM_Shader->Unbind();

		//g2p
		m_MLSMPM_G2P_Shader->Bind();
		m_MLSMPM_G2P_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_MLSMPM_G2P_Shader->setInt("gridBoundary", m_GridBoundary);
		m_MLSMPM_G2P_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
		m_MLSMPM_G2P_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_MLSMPM_G2P_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_MLSMPM_G2P_Shader->Dispatch(64 * 64, 1, 1);
		m_MLSMPM_G2P_Shader->Unbind();
	}

	void PhysicsLayer::SphMethod(float deltaTime)
	{
		auto& grid = m_Registry.get<C_SPH_Grid>(m_GridEntity);

		std::vector<uint32_t> zeros(grid.c_CellCount, 0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SPH_HashCountSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, grid.c_CellCount * sizeof(uint32_t), zeros.data());

		m_SPH_Grid1_Shader->Bind();
		m_SPH_Grid1_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_SPH_Grid1_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_SPH_Grid1_Shader->setFloat("cellSize", grid.c_CellSize);
		m_SPH_Grid1_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_SPH_Grid1_Shader->Dispatch(64 * 64, 1, 1);
		m_SPH_Grid1_Shader->Unbind();

		std::vector<uint32_t> countData(grid.c_CellCount);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SPH_HashCountSSBO);
		void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		memcpy(countData.data(), ptr, grid.c_CellCount * sizeof(uint32_t));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::vector<uint32_t> offsetData(grid.c_CellCount, 0);
		offsetData[0] = 0;
		for (int i = 1; i < grid.c_CellCount; ++i) {
			offsetData[i] = offsetData[i - 1] + countData[i - 1];
		}

		UpdateSSBO(m_SPH_OffsetSSBO, offsetData);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SPH_HashCount_2_SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, grid.c_CellCount * sizeof(uint32_t), zeros.data());

		m_SPH_Grid2_Shader->Bind();
		m_SPH_Grid2_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_SPH_Grid2_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_SPH_Grid2_Shader->setFloat("cellSize", grid.c_CellSize);
		m_SPH_Grid2_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_SPH_Grid2_Shader->Dispatch(64 * 64, 1, 1);
		m_SPH_Grid2_Shader->Unbind();

		m_SPH_Density_Shader->Bind();
		m_SPH_Density_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_SPH_Density_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_SPH_Density_Shader->setFloat("cellSize", grid.c_CellSize);
		m_SPH_Density_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_SPH_Density_Shader->Dispatch(64 * 64, 1, 1);
		m_SPH_Density_Shader->Unbind();

		m_SPH_Force_Shader->Bind();
		m_SPH_Force_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_SPH_Force_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_SPH_Force_Shader->setFloat("h", grid.c_CellSize);
		m_SPH_Force_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_SPH_Force_Shader->setFloat("restDensity", 20000);
		m_SPH_Force_Shader->setFloat("viscosity", 100);
		m_SPH_Force_Shader->setFloat("stiffness", 20);
		m_SPH_Force_Shader->setFloat("nearStiffness", 1);
		m_SPH_Force_Shader->Dispatch(64 * 64, 1, 1);
		m_SPH_Force_Shader->Unbind();

		m_SPH_Integrate_Shader->Bind();
		m_SPH_Integrate_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_SPH_Integrate_Shader->setFloat("cellSize", grid.c_CellSize);
		m_SPH_Integrate_Shader->setInt("gridBoundary", m_GridBoundary);
		m_SPH_Integrate_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_SPH_Integrate_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_SPH_Integrate_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_SPH_Integrate_Shader->Dispatch(64 * 64, 1, 1);
		m_SPH_Integrate_Shader->Unbind();
	}

	void PhysicsLayer::ReadPly(float deltaTime)
	{
	}
}

