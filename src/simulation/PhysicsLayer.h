#pragma once
#include <glad/glad.h>
#include "core/Layer.h"
#include <glm/glm.hpp>
#include "core/Macros.h"

namespace FMEditor {

	class PhysicsLayer : public Layer
	{
	public:
		PhysicsLayer(entt::registry& registry);
		void OnUpdate(float deltaTime);
	private:
		entt::registry& m_Registry;
		float PI;
		unsigned int positionSSBO;        // store particle position
		unsigned int gridIndexSSBO;       // store grid index
		unsigned int particleIndexSSBO;   
		unsigned int gridOffsetSSBO;      

		float kernelDensity(float r, float h);
		glm::vec3 kernelSpikyGradient(const glm::vec3 rVec, float h);
		float kernelViscosityLaplacian(float r, float h);

		template<typename T>
		inline unsigned int CreateSSBO(std::vector<T>& list)
		{
			GLuint ssbo;
			glGenBuffers(1, &ssbo);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
			FME_DEBUG_LOG_INFO("[PhysicsLayer.h]: SSBO created:{0},{1}", sizeof(T), list.size());
			return ssbo;
		}

		template<typename T>
		inline unsigned int UpdateSSBO(unsigned int ssbo,std::vector<T>& list)
		{
			glGenBuffers(1, &ssbo);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
			FME_DEBUG_LOG_INFO("[PhysicsLayer.h]: SSBO updated:{0},{1}", sizeof(T), list.size());
			return ssbo;
		}

		inline void BindSSBO(unsigned int ssbo) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		}
	};
}

