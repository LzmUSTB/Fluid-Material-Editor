#include "fmepch.h"
#include "Application.h"

namespace FMEditor {
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		if (s_Instance != nullptr) {
			FME_ERROR("Application already exist!");
			assert(0);
		}
		FME_INFO("Application initialized.");
		s_Instance = this;

		//m_Window = Scope<Window>(Window::Create());
		//m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//m_Window->SetVSync(false);
		//m_ImGuiLayer = new ImGuiLayer();
		//PushOverlay(m_ImGuiLayer);
	}

	void Application::Run()
	{
		while (m_Running) {

		}
	}
}
