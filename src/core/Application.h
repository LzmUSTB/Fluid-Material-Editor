#pragma once

#include "Macros.h"
#include "Window.h"
#include "LayerStack.h"
#include "imgui/ImguiLayer.h"

namespace FMEditor {
	class Application
	{
	public:
		Application();

		void Run();

		//void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }

	private:
		static Application* s_Instance;

		LayerStack m_LayerStack;
		ImguiLayer* m_ImguiLayer;
		float m_LastFrameTime = 0.0f;
		Scope<Window> m_Window;
	};
}


