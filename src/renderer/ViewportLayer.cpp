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
		m_ResolutionHeight = 720;
		m_ResolutionWidth = 1280;
	}

	void ViewportLayer::OnAttach()
	{
		m_Registry.emplace<C_Infomation>(m_Camera, "Camera");
		m_Registry.emplace<C_Camera>(m_Camera, 0.f, 0.f, 5.f, m_ResolutionWidth, m_ResolutionHeight);

		LoadResources();

		m_Renderer->Setup(m_ResolutionWidth, m_ResolutionHeight);

		auto objects = m_Registry.view<C_RenderObject>();
		for (auto entity : objects) {

			auto& object = objects.get<C_RenderObject>(entity);

			m_Renderer->SetupMesh(object.c_Mesh);
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
		glm::mat4 VP_Inv = glm::inverse(Projection * View);
		glm::mat4 View_Inv = glm::inverse(View);
		glm::mat4 Projection_Inv = glm::inverse(Projection);
		auto cameraPos = camera.c_Camera.GetPosition();

		m_Renderer->EnableDepthMask(true);
		m_SceneFrameBuffer->Bind();
		for (auto entity : objects) {
			auto& object = objects.get<C_RenderObject>(entity);

			if (object.c_RenderType == FME_SKYBOX) {
				m_Renderer->EnableDepthMask(false);
				m_skyboxShader->Bind();
				m_skyboxShader->setMat4("Projection", Projection);
				glm::mat4 view = glm::mat4(glm::mat3(View));
				m_skyboxShader->setMat4("View", view);
				m_Renderer->BindCubeMap(m_skyboxTexture);
				m_skyboxShader->setInt("Skybox", 0);
				m_Renderer->DrawMesh(object.c_Mesh);
			}
			else if (object.c_RenderType == FME_RIGIDBODY) {
				m_Renderer->EnableDepthMask(true);
				m_testShader->Bind();
				glm::mat4 Model = glm::mat4(1.0f);
				Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
				Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
				m_testShader->setMat4("Projection", Projection);
				m_testShader->setMat4("View", View);
				m_testShader->setMat4("Model", Model);
				m_Renderer->DrawMesh(object.c_Mesh);
			}
			else if (object.c_RenderType == FME_PARTICLE) {
				//for (auto entity : particleGroups) {
				//	auto& particle = particleGroups.get<C_ParticleGroup>(entity);
				//	glm::mat4 Model = glm::mat4(1.0f);
				//	//Model = glm::translate(Model, particle.c_PositionList);
				//	Model = glm::scale(Model, glm::vec3(0.1f, 0.1f, 0.1f));
				//	m_billboardShader->setMat4("Model", Model);
				//	m_billboardShader->setMat4("Model_i", glm::inverse(Model));
				//	m_Renderer->DrawMesh(object.c_Mesh);
				//}
			}
			else {
				auto& info = m_Registry.get<C_Infomation>(entity);
				FME_DEBUG_LOG_WARN("[ViewportLayer.cpp]Wrong c_RenderType in {0}", info.c_Name);
			}
		}
		m_SceneFrameBuffer->Unbind();

		auto particleGroups = m_Registry.view<C_ParticleGroup>();
		for (auto entity : particleGroups) {
			auto& particleGroup = particleGroups.get<C_ParticleGroup>(entity);
			glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(m_particleScale));

			// render particle thickness
			m_Renderer->EnableDepthMask(false);
			m_PingpongFBO_Thickness[0]->Bind();
			m_Renderer->BeginParticleRender();
			m_particleThicknessShader->Bind();
			m_particleThicknessShader->setMat4("Model", Model);
			m_particleThicknessShader->setMat4("Projection", Projection);
			m_particleThicknessShader->setMat4("View", View);
			m_particleThicknessShader->setFloat("ParticleRadius", m_particleSize);
			float coefficient = camera.c_Camera.GetHeight() / (2.f * glm::tan(camera.c_Camera.GetFovRadians() / 2));
			m_particleThicknessShader->setFloat("R_screen_coefficient", coefficient);
			m_Renderer->DrawPoints(m_particleSize, particleGroup.c_ParticleCount);
			m_Renderer->EndParticleRender();
			m_PingpongFBO_Thickness[0]->Unbind();

			// render particle depth
			m_Renderer->EnableDepthMask(true);
			m_particleDepthShader->Bind();
			m_particleDepthShader->setMat4("Model", Model);
			m_particleDepthShader->setMat4("Projection", Projection);
			m_particleDepthShader->setMat4("View", View);
			m_particleDepthShader->setVec3("CameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
			m_particleDepthShader->setFloat("ParticleRadius", m_particleSize);
			m_particleDepthShader->setFloat("Near", camera.c_Camera.GetNear());
			m_particleDepthShader->setFloat("Far", camera.c_Camera.GetFar());
			m_particleDepthShader->setFloat("R_screen_coefficient", coefficient);
			m_PingpongFBO_Depth[0]->Bind();
			m_Renderer->DrawPoints(m_particleSize, particleGroup.c_ParticleCount);
			m_Renderer->EndParticleRender();
			m_PingpongFBO_Depth[0]->Unbind();
		}

		m_Renderer->EnableDepthMask(false);
		ApplyFilter();

		// calculate normal
		m_NormalMapBuffer->Bind();
		m_normalMapShader->Bind();
		m_normalMapShader->setFloat("near", camera.c_Camera.GetNear());
		m_normalMapShader->setFloat("fov", camera.c_Camera.GetFovRadians());
		m_normalMapShader->setMat4("inv_View", View_Inv);
		m_normalMapShader->setMat4("inv_Projection", Projection_Inv);
		m_normalMapShader->setFloat("aspectRatio", camera.c_Camera.GetAspectRatio());
		m_PingpongTexture_Depth[0]->Bind();
		m_Renderer->DrawQuad();
		m_NormalMapBuffer->Unbind();

		// final process
		m_MainFramebuffer->Bind();
		m_finalProcessShader->Bind();

		m_finalProcessShader->setFloat("near", camera.c_Camera.GetNear());
		m_finalProcessShader->setFloat("fov", camera.c_Camera.GetFovRadians());
		m_finalProcessShader->setFloat("aspectRatio", camera.c_Camera.GetAspectRatio());
		m_finalProcessShader->setFloat("absorption", m_absorption);
		m_finalProcessShader->setFloat("refractOffsetAmount", m_refractOffset);
		m_finalProcessShader->setFloat("fresnelScale", m_fresnelScale);
		m_finalProcessShader->setInt("Shininess", m_shininess);
		m_finalProcessShader->setMat4("inv_View", View_Inv);
		m_finalProcessShader->setMat4("inv_Projection", Projection_Inv);
		m_finalProcessShader->setVec3("FluidColor", m_color[0], m_color[1], m_color[2]);
		m_finalProcessShader->setVec3("LightPosition", m_lightPosition[0], m_lightPosition[1], m_lightPosition[2]);
		m_finalProcessShader->setVec3("LightColor", m_lightColor[0], m_lightColor[1], m_lightColor[2]);

		m_SceneTexture->Bind(0);
		m_PingpongTexture_Thickness[0]->Bind(1);
		m_NormalMapTexture->Bind(2);
		m_PingpongTexture_Depth[0]->Bind(3);
		m_Renderer->BindCubeMap(m_skyboxTexture, 4);

		m_Renderer->DrawQuad();

		m_MainFramebuffer->Unbind();
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

		ShowNonStretchedImage(SwitchImage(m_textureShown), m_ResolutionWidth, m_ResolutionHeight);
		ImGui::End();

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;

		// GUI: openGL version etc. 
		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin("Renderer Info", &m_ShowRendererInfo, window_flags)) {
			ImGui::Text("OpenGL: %s", m_Renderer->Get_API_Version());	// TODO
			ImGui::Text("Device: %s", m_Renderer->Get_Device_Name());
			//ImGui::Text("Maximum SSBOs: %d", m_Renderer->Get_MaximumSSBO());
			ImGui::Text("FrameRate: %.f FPS", m_Counter->m_FPS);
		}
		ImGui::End();

		// GUI: camera information
		auto* camera = m_Registry.try_get<C_Camera>(m_Camera);
		ImGui::Begin("Camera Info");
		if (camera) {
			glm::vec3 cameraPos = camera->c_Camera.GetPosition();
			ImGui::Text("CameraPosition: ( %.2f, %.2f, %.2f )", cameraPos.x, cameraPos.y, cameraPos.z);
		}
		ImGui::End();

		// GUI: render option
		ImGui::Begin("Render Option");
		ImGui::SliderFloat("particle scale", &m_particleScale, 0.1f, 5.f, "%.3f");
		ImGui::SliderFloat("particle size", &m_particleSize, 0.0f, 0.05f, "%.3f");
		ImGui::SliderFloat("absorption", &m_absorption, 0.0f, 1.f, "%.3f");
		ImGui::ColorEdit3("fluid color", m_color);
		ImGui::SliderFloat("refract offset", &m_refractOffset, 0.0f, 1.f, "%.3f");
		ImGui::SliderFloat("filter threshold", &m_filterThreshold, 0.0f, 0.01f, "%.5f");
		ImGui::SliderFloat("filter offset", &m_filterOffset, 0.0f, 10.f, "%.3f");
		ImGui::SliderInt("filter blurSize", &m_blurSize, 0, 50);
		ImGui::SliderInt("filter iterations", &m_filterIterations, 0, 10);
		ImGui::SliderInt("render result", &m_textureShown, 0, 4);
		ImGui::End();

		// GUI: light option
		ImGui::Begin("Light Option");
		ImGui::ColorEdit3("light color", m_lightColor);
		ImGui::DragFloat3("light position", m_lightPosition);
		ImGui::SliderFloat("fresnel scale", &m_fresnelScale, 0.0f, 1.f, "%.3f");
		ImGui::SliderInt("specular shininess", &m_shininess, 1, 250);
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
		m_billboardShader = Shader::Create("assets/shaders/billboard.vert", "assets/shaders/billboard.frag");
		m_pointShader = Shader::Create("assets/shaders/point.vert", "assets/shaders/point.frag");
		m_particleThicknessShader = Shader::Create("assets/shaders/particle_thickness.vert", "assets/shaders/particle_thickness.frag");
		m_particleDepthShader = Shader::Create("assets/shaders/particle_thickness.vert", "assets/shaders/particle_depth.frag");
		m_narrowRangeFilterShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/narrow_range_filter.frag");
		m_gaussianFilterShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/gaussian_filter.frag");
		m_normalMapShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/calculate_normal.frag");
		m_finalProcessShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/finalProcess.frag");
		m_linearDepthShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/linear_depth.frag");
		m_2dFilterShader = Shader::Create("assets/shaders/postprocess.vert", "assets/shaders/2d_fix_filter.frag");

		std::vector<std::string> faces = {
			"assets/textures/cubemap/px.png", "assets/textures/cubemap/nx.png",
			"assets/textures/cubemap/py.png", "assets/textures/cubemap/ny.png",
			"assets/textures/cubemap/pz.png", "assets/textures/cubemap/nz.png"
		};
		m_skyboxTexture = m_Renderer->LoadCubeMap(faces);

		m_MainFramebuffer = FrameBuffer::Create();
		m_RenderTexture = Texture::Create(m_ResolutionWidth, m_ResolutionHeight, true, false);
		m_MainFramebuffer->AttachTexture(m_RenderTexture->GetID());

		m_SceneFrameBuffer = FrameBuffer::Create();
		m_SceneTexture = Texture::Create(m_ResolutionWidth, m_ResolutionHeight, true, false);
		m_SceneFrameBuffer->AttachTexture(m_SceneTexture->GetID());
		m_SceneFrameBuffer->CreateDepthBuffer(m_ResolutionWidth, m_ResolutionHeight);

		m_NormalMapBuffer = FrameBuffer::Create();
		m_NormalMapTexture = Texture::Create(m_ResolutionWidth, m_ResolutionHeight, true, true);
		m_NormalMapBuffer->AttachTexture(m_NormalMapTexture->GetID());

		for (int i = 0; i < 2; i++) {
			m_PingpongFBO_Thickness[i] = FrameBuffer::Create();
			m_PingpongFBO_Depth[i] = FrameBuffer::Create();

			m_PingpongTexture_Thickness[i] = Texture::Create(m_ResolutionWidth, m_ResolutionHeight, false, true);
			m_PingpongTexture_Depth[i] = Texture::Create(m_ResolutionWidth, m_ResolutionHeight, false, true);

			m_PingpongFBO_Thickness[i]->AttachTexture(m_PingpongTexture_Thickness[i]->GetID());
			m_PingpongFBO_Depth[i]->AttachTexture(m_PingpongTexture_Depth[i]->GetID());
		}
		m_PingpongFBO_Depth[0]->CreateDepthBuffer(m_ResolutionWidth, m_ResolutionHeight);
	}

	uint32_t ViewportLayer::SwitchImage(uint32_t option)
	{
		switch (option) {
		case 0:
			return m_RenderTexture->GetID();
		case 1:
			return m_NormalMapTexture->GetID();
		case 2:
			return m_PingpongTexture_Thickness[0]->GetID();
		case 3:
			return m_PingpongTexture_Depth[0]->GetID();
		case 4:
			return m_SceneTexture->GetID();
		default:
			return m_RenderTexture->GetID();
		}
	}

	void ViewportLayer::ApplyFilter()
	{
		m_PingpongFBO_Thickness[1]->ClearBuffer();
		m_PingpongFBO_Depth[1]->ClearBuffer();

		for (int i = 0; i < m_filterIterations; i++) {
			m_narrowRangeFilterShader->Bind();
			// horizontal filter
			m_narrowRangeFilterShader->setFloat("threshold", m_filterThreshold);
			m_narrowRangeFilterShader->setFloat("offsetFix", m_filterOffset);
			m_narrowRangeFilterShader->setInt("blurSize", m_blurSize);
			m_narrowRangeFilterShader->setBool("Horizontal", true);

			m_PingpongFBO_Thickness[1]->Bind(false);
			m_PingpongTexture_Thickness[0]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Thickness[1]->Unbind();

			m_PingpongFBO_Depth[1]->Bind(false);
			m_PingpongTexture_Depth[0]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Depth[1]->Unbind();

			// vertical filter
			m_narrowRangeFilterShader->setBool("Horizontal", false);

			m_PingpongFBO_Thickness[0]->Bind(false);
			m_PingpongTexture_Thickness[1]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Thickness[0]->Unbind();

			m_PingpongFBO_Depth[0]->Bind(false);
			m_PingpongTexture_Depth[1]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Depth[0]->Unbind();

			m_narrowRangeFilterShader->Unbind();

			// fix glitch
			m_2dFilterShader->Bind();
			m_PingpongFBO_Thickness[1]->Bind(false);
			m_PingpongTexture_Thickness[0]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Thickness[1]->Unbind();

			m_PingpongFBO_Depth[1]->Bind(false);
			m_PingpongTexture_Depth[0]->Bind();
			m_Renderer->DrawQuad();
			m_PingpongFBO_Depth[1]->Unbind();
			m_2dFilterShader->Unbind();
		}
	}

	void ViewportLayer::CalculateNormal()
	{

	}

	void ViewportLayer::FinalProcess()
	{

	}
}
