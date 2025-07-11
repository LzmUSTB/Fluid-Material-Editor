#include "fmepch.h"
#include "PhysicsLayer.h"
#include "core/ecs/Components.h"
#include "tinyply/tinyply.h"

namespace FMEditor {
	PhysicsLayer::PhysicsLayer(entt::registry& registry) :
		Layer("PhysicsLayer"), m_Registry(registry),
		m_Paused(true),
		m_TimeScale(1.f),
		m_GridBoundary(30),
		m_Stiffness(10.f),
		m_RestDensity(0.25f),
		m_Viscosity(0.1f),
		m_SimulationMode(0),
		m_UsePly(false),
		m_CurrentFrame(0),
		m_FrameCount(0),
		m_ParticleCount(0),
		m_FrameTime(0.3),
		m_CurrentFrameTime(0)
	{
		PI = glm::pi<float>();
	}

	void PhysicsLayer::OnUpdate(float deltaTime)
	{
		if (!m_Paused) {
			switch (m_SimulationMode)
			{
			case 0:
				break;
			case 1:
				MlsmpmMethod(deltaTime);
				break;
			case 2:
				SphMethod(deltaTime);
				break;
			case 3:
				RunPly(deltaTime);
				break;
			default:
				break;
			}
		}
	}

	void PhysicsLayer::OnAttach()
	{
		LoadResources();
	}

	void PhysicsLayer::OnImguiRender()
	{

		ImGui::Begin("Simulation setting");

		if (ImGui::Button("MLS-MPM")) {
			FME_DEBUG_LOG_TRACE("mls-mpm method");
			LoadReadources_mlsmpm();
			m_SimulationMode = 1;
		}

		if (ImGui::Button("SPH")) {
			FME_DEBUG_LOG_TRACE("sph method");
			LoadReadources_sph();
			m_SimulationMode = 2;
		}

		if (ImGui::Button("Load PLY")) {
			FME_DEBUG_LOG_TRACE("load ply file");
			std::string folder = "assets/ply";
			m_PlyPointFrames = LoadReadources_ply(folder);
			CreateParticlesFromPly();
			m_UsePly = true;
			m_SimulationMode = 3;
		}

		auto* particleGroup = m_Registry.try_get<C_ParticleGroup>(m_ParticleEntity);
		if (ImGui::Button("Reset Position")) {
			if (particleGroup) {
				if (m_SimulationMode == 1 || m_SimulationMode == 2) {
					FME_DEBUG_LOG_TRACE("Reset Particle Position and Velocity");
					FME_LOG_TRACE("[PhysicsLayer.cpp]Reset Particle Position and Velocity");
					UpdateSSBO(m_PositionSSBO, particleGroup->c_PositionList);
					UpdateSSBO(m_VelocitySSBO, particleGroup->c_VelocityList);
					UpdateSSBO(m_AffineVelocityFieldSSBO, particleGroup->c_AffineVelocityField);
				}
				if (m_SimulationMode == 3) {
					UpdateSSBO(m_PositionSSBO, m_PlyPointFrames[0]);
					m_CurrentFrame = 0;
				}
			}
		}

		ImGui::Checkbox("Paused", &m_Paused);

		ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.1f, 1.f, "%.3f");
		ImGui::SliderFloat("Stiffness", &m_Stiffness, 0.f, 1000.f, "%.3f");
		ImGui::SliderFloat("Density", &m_RestDensity, 0.15f, 1.f, "%.3f");
		ImGui::SliderFloat("Viscosity", &m_Viscosity, 0.f, 1000.f, "%.3f");
		ImGui::SliderFloat("elastic:mu", &m_Mu, 0.f, 100.f, "%.3f");
		ImGui::SliderFloat("elastic:lambda", &m_Lambda, 0.f, 100.f, "%.3f");
		ImGui::SliderInt("Grid Boundary", &m_GridBoundary, 1, 40, "%d");
		ImGui::End();

		if (m_UsePly) {
			ImGui::Begin("Ply");
			ImGui::Text("Particle Count: %d", m_ParticleCount);
			ImGui::SliderInt("Current Frame", &m_CurrentFrame, 0, m_FrameCount - 1, "%d");
			ImGui::SliderFloat("Frame Time", &m_FrameTime, 0.01f, 1.f, "%.3f");
			ImGui::End();
		}
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

	void PhysicsLayer::LoadReadources_mlsmpm()
	{
		if (m_Registry.valid(m_ParticleEntity))
			m_Registry.destroy(m_ParticleEntity);

		m_ParticleEntity = m_Registry.create();

		int LENGTH = 64;
		int WIDTH = 64;
		int HEIGHT = 64;
		float interval = 0.025;
		float initOffset = 0;
		int particleCount = WIDTH * HEIGHT * LENGTH;
		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(m_ParticleEntity, particleCount, 1000.f);
		unsigned int index = 0;

		for (int i = 0; i < LENGTH / 2; ++i) {
			for (int j = 0; j < HEIGHT; ++j) {
				for (int k = 0; k < WIDTH; ++k) {
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

		for (int i = 0; i < LENGTH / 2; ++i) {
			for (int j = 0; j < HEIGHT; ++j) {
				for (int k = 0; k < WIDTH; ++k) {
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

		FME_DEBUG_LOG_TRACE("mlsmpm gridOrigin: {0}, {1}, {2}", grid.c_GridOrigin.x, grid.c_GridOrigin.y, grid.c_GridOrigin.z);

		// mls-mpm
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

	void PhysicsLayer::LoadReadources_sph()
	{
		if (m_Registry.valid(m_ParticleEntity))
			m_Registry.destroy(m_ParticleEntity);
		// particles
		m_ParticleEntity = m_Registry.create();

		int LENGTH = 32;
		int WIDTH = 32;
		int HEIGHT = 32;
		float interval = 0.025;
		float initOffset = 0;
		int particleCount = WIDTH * HEIGHT * LENGTH;
		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(m_ParticleEntity, particleCount, 1000.f);
		unsigned int index = 0;

		for (int i = 0; i < LENGTH / 2; ++i) {
			for (int j = 0; j < HEIGHT; ++j) {
				for (int k = 0; k < WIDTH; ++k) {
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

		for (int i = 0; i < LENGTH / 2; ++i) {
			for (int j = 0; j < HEIGHT; ++j) {
				for (int k = 0; k < WIDTH; ++k) {
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

		glm::vec3 SPHgridRange = glm::vec3(5.76, 5.76, 5.76);
		float SPHcellSize = 3.f * interval;
		glm::vec3 SPHgridResolution = glm::ceil(SPHgridRange / SPHcellSize);
		auto& grid_sph = m_Registry.emplace<C_SPH_Grid>(m_GridEntity, SPHgridResolution, SPHcellSize, particleCount);

		FME_DEBUG_LOG_TRACE("sph gridOrigin: {0}, {1}, {2}", grid_sph.c_GridOrigin.x, grid_sph.c_GridOrigin.y, grid_sph.c_GridOrigin.z);

		FME_DEBUG_LOG_TRACE("sph gridResolution: {0}, {1}, {2}", SPHgridResolution.x, SPHgridResolution.y, SPHgridResolution.z);

		m_PositionSSBO = CreateSSBO(particleGroup.c_PositionList);
		m_VelocitySSBO = CreateSSBO(particleGroup.c_VelocityList);

		// sph
		m_SPH_HashCountSSBO = CreateSSBO(grid_sph.c_HashCount);
		m_SPH_HashCount_2_SSBO = CreateSSBO(grid_sph.c_HashCount2);
		m_SPH_IndexSSBO = CreateSSBO(grid_sph.c_IndexArr);
		m_SPH_OffsetSSBO = CreateSSBO(grid_sph.c_OffsetArr);
		m_SPH_ForceSSBO = CreateSSBO(grid_sph.c_Force);

		BindSSBO(m_PositionSSBO, 0);
		BindSSBO(m_VelocitySSBO, 1);

		BindSSBO(m_SPH_HashCountSSBO, 5);
		BindSSBO(m_SPH_HashCount_2_SSBO, 6);
		BindSSBO(m_SPH_IndexSSBO, 7);
		BindSSBO(m_SPH_OffsetSSBO, 8);
		BindSSBO(m_DeformationGradientSSBO, 9);
	}

	std::vector<std::vector<glm::vec4>> PhysicsLayer::LoadReadources_ply(const std::string& folderPath)
	{
		if (m_Registry.valid(m_ParticleEntity))
			m_Registry.destroy(m_ParticleEntity);

		std::vector<std::pair<int, std::filesystem::path>> sortedFiles;
		std::regex pattern(R"(frame_(\d+)\.ply)");

		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			const std::string filename = entry.path().filename().string();
			std::smatch match;

			if (std::regex_match(filename, match, pattern)) {
				int frameNumber = std::stoi(match[1].str());
				sortedFiles.emplace_back(frameNumber, entry.path());
			}
		}

		std::sort(sortedFiles.begin(), sortedFiles.end(),
			[](const auto& a, const auto& b) {
				return a.first < b.first;
			});

		std::vector<std::vector<glm::vec4>> frames;
		for (const auto& [frameNumber, path] : sortedFiles) {
			std::vector<glm::vec4> positions = LoadPlyPositions(path.string());
			frames.push_back(std::move(positions));
			std::cout << "Loaded frame " << frameNumber << " from " << path << "\n";
		}

		return frames;
	}

	void PhysicsLayer::MlsmpmMethod(float deltaTime)
	{
		auto& grid = m_Registry.get<C_MLSMPM_Grid>(m_GridEntity);
		// clear grid
		ClearSSBO(m_GridSSBO);

		// p2g
		m_MLSMPM_P2G_Shader->Bind();
		m_MLSMPM_P2G_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_MLSMPM_P2G_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_MLSMPM_P2G_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_MLSMPM_P2G_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
		m_MLSMPM_P2G_Shader->setVec3("gridOrigin", grid.c_GridOrigin);
		m_MLSMPM_P2G_Shader->Dispatch(64 * 64, 1, 1);
		m_MLSMPM_P2G_Shader->Unbind();

		// p2g 2
		m_MLSMPM_P2G_2_Shader->Bind();
		m_MLSMPM_P2G_2_Shader->setInt("particleCount", grid.c_ParticleCount);
		m_MLSMPM_P2G_2_Shader->setIVec3("gridRes", grid.c_GridResolution);
		m_MLSMPM_P2G_2_Shader->setFloat("deltaTime", deltaTime * m_TimeScale);
		m_MLSMPM_P2G_2_Shader->setFloat("gridSpacing", grid.c_GridSpacing);
		m_MLSMPM_P2G_2_Shader->setFloat("stiffness", m_Stiffness);
		m_MLSMPM_P2G_2_Shader->setFloat("rest_density", m_RestDensity);
		m_MLSMPM_P2G_2_Shader->setFloat("dynamic_viscosity", m_Viscosity);
		m_MLSMPM_P2G_2_Shader->setFloat("elastic_mu", m_Mu);
		m_MLSMPM_P2G_2_Shader->setFloat("elastic_lambda", m_Lambda);
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
		m_MLSMPM_G2P_Shader->setInt("particleCount", grid.c_ParticleCount);
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
		m_SPH_Force_Shader->setFloat("restDensity", m_RestDensity);
		m_SPH_Force_Shader->setFloat("viscosity", m_Viscosity);
		m_SPH_Force_Shader->setFloat("stiffness", m_Stiffness);
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

	void PhysicsLayer::RunPly(float deltaTime)
	{
		if (m_CurrentFrame >= m_FrameCount)return;

		m_CurrentFrameTime += deltaTime;
		if (m_CurrentFrameTime >= m_FrameTime) {

			m_CurrentFrameTime = 0;
			UpdateSSBO(m_PositionSSBO, m_PlyPointFrames[m_CurrentFrame]);
			m_CurrentFrame++;
		}
	}

	std::vector<glm::vec4> PhysicsLayer::LoadPlyPositions(const std::string& path)
	{
		std::vector<glm::vec4> positions;

		try {
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Failed to open PLY file: " << path << "\n";
				return positions;
			}

			tinyply::PlyFile plyFile;
			plyFile.parse_header(file);

			std::shared_ptr<tinyply::PlyData> verts;
			try {
				verts = plyFile.request_properties_from_element("vertex", { "x", "y", "z" });
			}
			catch (const std::exception& e) {
				std::cerr << "Failed to get vertex properties from: " << path << "\n";
				return positions;
			}

			plyFile.read(file);

			if (verts) {
				const size_t numVerts = verts->count;
				const float* src = reinterpret_cast<const float*>(verts->buffer.get());

				positions.reserve(numVerts);
				for (size_t i = 0; i < numVerts; ++i) {
					glm::vec4 pos(src[i * 3 + 0], src[i * 3 + 1], src[i * 3 + 2], 0);
					positions.push_back(pos);
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Exception reading PLY file " << path << ": " << e.what() << "\n";
		}

		return positions;
	}

	void PhysicsLayer::CreateParticlesFromPly()
	{
		m_FrameCount = m_PlyPointFrames.size();
		m_ParticleCount = m_PlyPointFrames[0].size();
		m_CurrentFrame = 0;

		m_ParticleEntity = m_Registry.create();
		auto& particleGroup = m_Registry.emplace<C_ParticleGroup>(m_ParticleEntity, m_ParticleCount, 1000.f);

		m_PositionSSBO = CreateSSBO(m_PlyPointFrames[m_CurrentFrame]);
		BindSSBO(m_PositionSSBO, 0);
	}
}

