#include "fmepch.h"
#include "ViewportLayer.h"
#include "core/Macros.h"

namespace FMEditor {
	ViewportLayer::ViewportLayer()
		: Layer("ViewportLayer"), m_ShowRendererInfo(true)
	{
		m_Renderer = Renderer::Create();
		m_Counter = CreateScope<FrameRateCounter>(1.0f);
	}

	void ViewportLayer::OnAttach()
	{
		// TODO
	}

	void ViewportLayer::OnDetach()
	{
		// TODO
	}

	void ViewportLayer::OnUpdate(float deltaTime)
	{
		m_Counter->Update(deltaTime);
		if (m_IsFirstFrame) return;
		m_Renderer->BeginScene();
		// TODO
		m_Renderer->EndScene();
	}

	void ViewportLayer::OnImguiRender()
	{
		ImGui::Begin("Viewport");
		//ImGui::Checkbox("ShowInformation", &m_ShowRendererInfo);

		//get window size without label
		m_WindowSize = ImGui::GetContentRegionAvail();

		if (m_IsFirstFrame)
		{
			m_Renderer->Setup((int)m_WindowSize.x, (int)m_WindowSize.y);
			m_TextureID = m_Renderer->GetRenderTexture();
			m_IsFirstFrame = false;
		}
		else
		{
			ImGui::Image(m_TextureID, m_WindowSize);
		}
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

}
