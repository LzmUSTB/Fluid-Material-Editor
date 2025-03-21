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

		m_Window = Scope<Window>(Window::Create());
		//m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_ImguiLayer = new ImguiLayer();
		PushOverlay(m_ImguiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		while (m_Running) {

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			m_ImguiLayer->Begin();

			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImguiLayer->End();

			m_Window->OnUpdate();
		}
	}
}
