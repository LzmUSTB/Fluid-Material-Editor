#include "fmepch.h"
#include "EntityLayer.h"
#include "imgui.h"
#include "Components.h"

namespace FMEditor {
	EntityLayer::EntityLayer() :Layer("EntityLayer")
	{
		LoadResources();
	}

	void EntityLayer::OnImguiRender()
	{
		ImGui::Begin("Entity Inspector");
		auto view = m_Registry.view<C_Infomation>();
		ImGui::Text("entity list:");
		for (auto entity : view) {
			//ImGui::DragFloat3("Position", glm::value_ptr(tf->position));
			auto& info = view.get<C_Infomation>(entity);
			ImGui::Text(info.c_Name.c_str());
		}
		ImGui::End();
	}

	void EntityLayer::LoadResources()
	{
		// test entity
		//entt::entity entity = m_Registry.create();
		//m_Registry.emplace<C_Infomation>(entity, "square");
		//float vertices[] = {
		//	0.5f, 0.5f, 0.0f,
		//	0.5f, -0.5f, 0.0f,
		//	-0.5f, -0.5f, 0.0f,
		//	-0.5f, 0.5f, 0.0f
		//};
		//unsigned int indices[] = {
		//	0, 1, 3,
		//	1, 2, 3
		//};
		//Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices));
		//m_Registry.emplace<C_RenderObject>(entity, FME_RIGIDBODY, mesh);

		// particles
		//for (int i = 0; i < 5000; i++) {
		//	entt::entity particle = m_Registry.create();
		//}

		// skybox
		entt::entity skybox = m_Registry.create();
		m_Registry.emplace<C_Infomation>(skybox, "skybox");
		float skyboxVertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f
		};
		unsigned int skyboxIndices[] = {
			0, 1, 2,
			2, 3, 0,


			1, 5, 6,
			6, 2, 1,


			5, 4, 7,
			7, 6, 5,


			4, 0, 3,
			3, 7, 4,

			3, 2, 6,
			6, 7, 3,

			4, 5, 1,
			1, 0, 4
		};
		Mesh skyboxMesh(skyboxVertices, sizeof(skyboxVertices), skyboxIndices, sizeof(skyboxIndices));
		m_Registry.emplace<C_RenderObject>(skybox, FME_SKYBOX, skyboxMesh);
	}


}