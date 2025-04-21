#pragma once
#include <glm/glm.hpp>

namespace FMEditor {

	struct Vertex {
		Vertex(float x, float y, float z) {
			Position = glm::vec3(x, y, z);
		}
		// position
		glm::vec3 Position;
		// normal
		//glm::vec3 Normal;
		// texCoords
		//glm::vec2 TexCoords;
		// tangent
		//glm::vec3 Tangent;
		// bitangent
		//glm::vec3 Bitangent;

		//bone indexes which will influence this vertex
		//int m_BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		//float m_Weights[MAX_BONE_INFLUENCE];
	};

	struct Mesh
	{
		std::vector<float> vertices;   
		std::vector<unsigned int> indices;
		unsigned int VAO, VBO, EBO;

		Mesh(const float* verts, size_t vertSize, const unsigned int* inds, size_t indSize) 
			: vertices(verts, verts + vertSize / sizeof(float)), 
			indices(inds, inds + indSize / sizeof(unsigned int))
		{
			VAO = 0;
			VBO = 0;
			EBO = 0;
		}
	};


}

