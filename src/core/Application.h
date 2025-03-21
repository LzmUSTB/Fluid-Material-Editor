#pragma once

#include "Macros.h"

namespace FMEditor {
	class Application
	{
	public:
		Application();

		void Run();

		//void OnEvent(Event& e);
		//void PushLayer(Layer* layer);
		//void PushOverlay(Layer* layer);
		//Window& GetWindow() { return *m_Window; }
	private:
		static Application* s_Instance;
		bool m_Running = true;
		//LayerStack m_LayerStack;
		//ImGuiLayer* m_ImGuiLayer;
		float m_LastFrameTime = 0.0f;
	};
}


