#include "fmepch.h"
#include "ViewportLayer.h"
#include "core/ecs/Components.h"

namespace FMEditor {
	ViewportLayer::ViewportLayer(entt::registry& registry)
		: Layer("ViewportLayer"), m_ShowRendererInfo(true), m_Registry(registry)
	{
		m_Renderer = Renderer::Create();
		m_Counter = CreateScope<FrameRateCounter>(1.0f);
		m_Camera = m_Registry.create();
	}

	void ViewportLayer::OnAttach()
	{
		m_Registry.emplace<C_Infomation>(m_Camera, "Camera");
		m_Registry.emplace<C_Camera>(m_Camera, 0.f, 0.f, 5.f, 1280, 720);

		LoadResources();

		m_Renderer->Setup(1280, 720);
		m_TextureID = m_Renderer->GetRenderTexture();

		auto objects = m_Registry.view<C_RenderObject>();
		for (auto entity : objects) {

			auto& object = objects.get<C_RenderObject>(entity);

			m_Renderer->SetupMesh(object.c_mesh);
		}
	}

	void ViewportLayer::OnDetach()
	{
		// TODO
		//m_Registry.destroy(m_Camera);
	}

	void ViewportLayer::OnUpdate(float deltaTime)
	{
		m_Counter->Update(deltaTime);

		auto& camera = m_Registry.get<C_Camera>(m_Camera);
		auto& objects = m_Registry.view<C_RenderObject>();
		//camera.c_Camera.ProcessWindowResize(m_WindowSize.x, m_WindowSize.y);
		if (io_MouseWheelOffset) {
			camera.c_Camera.ProcessMouseScroll(io_MouseWheelOffset);
		}

		if (io_MousePosOffset.x || io_MousePosOffset.y) {
			camera.c_Camera.ProcessMouseDrag(io_MousePosOffset.x, io_MousePosOffset.y);
		}

		glm::mat4 Projection = camera.c_Camera.GetProjectionMatrix();
		glm::mat4 View = camera.c_Camera.GetViewMatrix();

		// TODO: make frameBufferID dynamic
		m_Renderer->BeginScene();

		for (auto entity : objects) {
			auto& object = objects.get<C_RenderObject>(entity);

			if (object.c_renderType == FME_SKYBOX) {
				m_Renderer->EnableDepthMask(false);
				m_skyboxShader->Bind();
				m_skyboxShader->setMat4("Projection", Projection);
				glm::mat4 view = glm::mat4(glm::mat3(View));
				m_skyboxShader->setMat4("View", view);
				m_Renderer->BindCubeMap(m_skyboxTexture);
				m_skyboxShader->setInt("Skybox", 0);
				m_Renderer->DrawMesh(object.c_mesh);
				m_Renderer->EnableDepthMask(true);
			}
			if (object.c_renderType == FME_RIGIDBODY) {
				m_testShader->Bind();
				glm::mat4 Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
				Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
				m_testShader->setMat4("Projection", Projection);
				m_testShader->setMat4("View", View);
				m_testShader->setMat4("Model", Model);
				m_Renderer->DrawMesh(object.c_mesh);
			}
			else {

			}
		}

		m_Renderer->EndScene();
	}

	void ViewportLayer::OnImguiRender()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Viewport");
		//ImGui::Checkbox("ShowInformation", &m_ShowRendererInfo);
		//get window size without label
		m_WindowSize = ImGui::GetContentRegionAvail();

		if (ImGui::IsWindowFocused()) {
			io_MouseWheelOffset = io.MouseWheel;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowFocused()) {
			io_MousePosOffset.x = io.MousePos.x - io_LastMousePos.x;
			io_MousePosOffset.y = io.MousePos.y - io_LastMousePos.y;
			io_LastMousePos = io.MousePos;
		}
		else {
			io_LastMousePos = io.MousePos;
			io_MousePosOffset = ImVec2(0, 0);
		}
		//if (m_IsFirstFrame)
		//{
		//	m_Renderer->Setup((int)m_WindowSize.x, (int)m_WindowSize.y);
		//	m_TextureID = m_Renderer->GetRenderTexture();
		//	m_IsFirstFrame = false;
		//}
		ShowNonStretchedImage(m_TextureID, 1280, 720);
		ImGui::End();

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;

		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin("Renderer Info", &m_ShowRendererInfo, window_flags)) {
			ImGui::Text("OpenGL: %s", m_Renderer->Get_API_Version());	// TODO
			ImGui::Text("Device: %s", m_Renderer->Get_Device_Name());
			ImGui::Text("FrameRate: %.f FPS", m_Counter->m_FPS);
		}
		ImGui::End();
	}

	void ViewportLayer::ShowNonStretchedImage(ImTextureID tex_id, int image_width, int image_height) {
		ImVec2 avail_size = m_WindowSize;

		float aspect_ratio = static_cast<float>(image_width) / image_height;
		ImVec2 fit_size;
		if (avail_size.x / aspect_ratio <= avail_size.y) {
			fit_size = ImVec2(avail_size.x, avail_size.x / aspect_ratio);
		}
		else {
			fit_size = ImVec2(avail_size.y * aspect_ratio, avail_size.y);
		}

		ImVec2 cursor_pos = ImGui::GetCursorPos();
		float offset_x = (avail_size.x - fit_size.x) * 0.5f;
		float offset_y = (avail_size.y - fit_size.y) * 0.5f;

		ImGui::SetCursorPos(ImVec2(cursor_pos.x + offset_x, cursor_pos.y + offset_y));

		ImGui::Image(tex_id, fit_size, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::SetCursorPos(cursor_pos);
	}

	void ViewportLayer::LoadResources()
	{
		m_skyboxShader = Shader::Create("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
		m_testShader = Shader::Create("assets/shaders/test.vert", "assets/shaders/test.frag");
		std::vector<std::string> faces = {
			"assets/textures/cubemap/px.png", "assets/textures/cubemap/nx.png",
			"assets/textures/cubemap/py.png", "assets/textures/cubemap/ny.png",
			"assets/textures/cubemap/pz.png", "assets/textures/cubemap/nz.png"
		};
		m_skyboxTexture = m_Renderer->LoadCubeMap(faces);
	}

}
