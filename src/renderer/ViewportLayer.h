#pragma once
#include "core/Layer.h"
#include "Renderer.h"
#include "imgui.h"

namespace FMEditor {
	struct FrameRateCounter {
		FrameRateCounter(float interval) {
			m_UpdateInterval = interval;
			m_FPS = 0;
			m_DeltaTime = 0;
			m_FrameCount = 0;
		};
		unsigned int m_FrameCount;
		float m_FPS;
		float m_UpdateInterval;
		float m_DeltaTime;
		void Update(float deltaTime) {
			++m_FrameCount;
			m_DeltaTime += deltaTime;
			if (m_DeltaTime >= m_UpdateInterval) {
				m_FPS = m_FrameCount / m_DeltaTime;
				m_FrameCount = 0;
				m_DeltaTime = 0;
			}
		}
	};

	class ViewportLayer : public Layer
	{
	public:
		ViewportLayer();
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnImguiRender() override;
	private:
		bool m_IsFirstFrame = true;
		Scope<Renderer> m_Renderer;
		ImVec2 m_WindowSize;
		ImTextureID m_TextureID;
		Scope<FrameRateCounter> m_Counter;
		//imgui
		bool m_ShowRendererInfo;
	};

}
