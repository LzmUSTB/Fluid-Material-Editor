#include "fmepch.h"
#include "Application.h"

namespace FMEditor {
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		if (s_Instance != nullptr) {
			FME_LOG_ERROR("[Application.cpp]: Application already exist!");
			FME_DEBUG_LOG_ERROR("[Application.cpp]: Application already exist!");
			FME_DEBUG_ASSERT(0);
		}
		FME_DEBUG_LOG_INFO("[Application.cpp]: Application created.");
		FME_LOG_INFO("[Application.cpp]: Application created.");
		s_Instance = this;

		m_Window = Scope<Window>(Window::Create());
		m_ImguiLayer = new ImguiLayer();
		PushOverlay(m_ImguiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
		FME_DEBUG_LOG_INFO("[Application.cpp]: Layer::{0} pushed.", layer->GetName());
		FME_LOG_INFO("[Application.cpp]: Layer::%s pushed.", layer->GetName().c_str());
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
		FME_DEBUG_LOG_INFO("[Application.cpp]: Layer::{0} pushed overlay.", layer->GetName());
		FME_LOG_INFO("[Application.cpp]: Layer::%s pushed overlay.", layer->GetName().c_str());
	}

	void Application::Run()
	{
		while (m_Window->OnUpdate()) {

			// frame time //----------------------------------
			float currentFrameTime = m_Window->GetFrameTime();
			float deltaFrameTime = currentFrameTime - m_LastFrameTime;
			m_LastFrameTime = currentFrameTime;

			// layer update //----------------------------------
			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(deltaFrameTime);
			}

			// imgui rendering //-------------------------------
			m_ImguiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImguiRender();
			}
			m_ImguiLayer->End();
		}
	}
}
