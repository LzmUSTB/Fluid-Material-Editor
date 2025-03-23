#include "fmepch.h"
#include "ViewportLayer.h"
#include "core/Macros.h"

namespace FMEditor {
	ViewportLayer::ViewportLayer()
		: Layer("ViewportLayer")
	{
		m_Renderer = Renderer::Create();
	}

	void ViewportLayer::OnAttach()
	{
		// TODO
		FME_INFO("viewportLayer attached");
	}

	void ViewportLayer::OnDetach()
	{
		// TODO
		FME_INFO("viewportLayer detached");
	}

	void ViewportLayer::OnUpdate()
	{
		if (m_IsFirstFrame) return;
		m_Renderer->BeginScene();
		// TODO
		m_Renderer->EndScene();
	}

	void ViewportLayer::OnImguiRender()
	{
		ImGui::Begin("Viewport");
		//get window size without label
		m_WindowSize = ImGui::GetContentRegionAvail();
		if (m_IsFirstFrame)
		{
			m_Renderer->Setup(m_WindowSize.x, m_WindowSize.y);
			m_TextureID = m_Renderer->GetRenderTexture();
			m_IsFirstFrame = false;
		}
		else
		{
			ImGui::Image(m_TextureID, m_WindowSize);
		}
		ImGui::End();
	}

}
