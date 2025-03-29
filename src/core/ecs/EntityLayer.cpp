#include "fmepch.h"
#include "EntityLayer.h"
#include "imgui.h"
#include "Components.h"

namespace FMEditor {
	EntityLayer::EntityLayer() :Layer("EntityLayer")
	{
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<C_Infomation>(entity, "test object");
		float vertices[] = {
			0.5f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices));

		m_Registry.emplace<C_RenderObject>(entity, mesh);
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

	//entt::entity EntityLayer::AddEntity(std::string name)
	//{
	//	entt::entity entity = m_Registry.create();
	//	m_Registry.emplace<C_Infomation>(entity, name);
	//	return entity;
	//}
}