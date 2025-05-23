#pragma once
#include <glad/glad.h>
#include "core/Layer.h"
#include <glm/glm.hpp>
#include "core/Macros.h"
#include "platform/openGL/OpenGL_ComputeShader.h"
#include "imgui.h"

namespace FMEditor {

	class PhysicsLayer : public Layer
	{
	public:
		PhysicsLayer(entt::registry& registry);
		void OnUpdate(float deltaTime) override;
		void OnAttach() override;
		void OnImguiRender() override;
	private:
		void LoadResources();
		void MlsmpmMethod(float deltaTime);
		void SphMethod(float deltaTime);
		void ReadPly(float deltaTime);

		template<typename T>
		inline uint32_t CreateSSBO(std::vector<T>& list)
		{
			GLuint ssbo;
			glGenBuffers(1, &ssbo);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
			FME_DEBUG_LOG_INFO("[PhysicsLayer.h]: SSBO created: typeSize: {0}, ListSize: {1}", sizeof(T), list.size());
			return ssbo;
		}

		template<typename T>
		inline unsigned int UpdateSSBO(unsigned int ssbo, std::vector<T>& list)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
			//FME_DEBUG_LOG_INFO("[PhysicsLayer.h]: SSBO updated:{0},{1}", sizeof(T), list.size());
			return ssbo;
		}

		inline void BindSSBO(unsigned int ssbo, int index) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
		}

		inline void ClearSSBO(unsigned int ssbo) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glm::vec4 zero(0.0f);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RGBA, GL_FLOAT, &zero);
		}

	private:
		entt::registry& m_Registry;
		float PI;
		entt::entity m_GridEntity;
		entt::entity m_ParticleEntity;

		// particle status
		uint32_t m_PositionSSBO;
		uint32_t m_VelocitySSBO;
		uint32_t m_GridSSBO;
		uint32_t m_DeformationGradientSSBO;
		uint32_t m_AffineVelocityFieldSSBO;

		uint32_t m_SPH_HashCountSSBO;
		uint32_t m_SPH_HashCount_2_SSBO;
		uint32_t m_SPH_IndexSSBO;
		uint32_t m_SPH_OffsetSSBO;
		uint32_t m_SPH_ForceSSBO;

		// compute shaders
		Scope<OpenGL_ComputeShader> m_MLSMPM_P2G_Shader;
		Scope<OpenGL_ComputeShader> m_MLSMPM_P2G_2_Shader;
		Scope<OpenGL_ComputeShader> m_MLSMPM_SIM_Shader;
		Scope<OpenGL_ComputeShader> m_MLSMPM_G2P_Shader;
		Scope<OpenGL_ComputeShader> m_SPH_Grid1_Shader;
		Scope<OpenGL_ComputeShader> m_SPH_Grid2_Shader;
		Scope<OpenGL_ComputeShader> m_SPH_Density_Shader;
		Scope<OpenGL_ComputeShader> m_SPH_Force_Shader;
		Scope<OpenGL_ComputeShader> m_SPH_Integrate_Shader;

		// simulation settings
		bool m_Paused;
		float m_TimeScale;
		int m_GridBoundary;
		float m_Stiffness;
		float m_RestDensity;
		float m_Viscosity;
	};
}

