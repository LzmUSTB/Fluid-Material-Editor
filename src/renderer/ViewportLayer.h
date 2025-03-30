#pragma once
#include "core/Layer.h"
#include "Renderer.h"
#include "imgui.h"
#include "entt/entity/registry.hpp"
#include "core/Macros.h"
#include "renderer/Shader.h"

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
		ViewportLayer(entt::registry& registry);
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnImguiRender() override;
	private:
		void LoadResources();
		bool m_IsFirstFrame = true;
		Scope<Renderer> m_Renderer;
		ImVec2 m_WindowSize;
		ImTextureID m_TextureID;
		Scope<FrameRateCounter> m_Counter;

		// entity
		entt::registry& m_Registry;
		entt::entity m_Camera;


		// imgui
		void ShowNonStretchedImage(ImTextureID tex_id, int image_width, int image_height);
		bool m_ShowRendererInfo;
		float io_MouseWheelOffset;
		ImVec2 io_LastMousePos;
		ImVec2 io_MousePosOffset;

		// shader & texture
		Scope<Shader> m_skyboxShader;
		Scope<Shader> m_testShader;
		unsigned int m_skyboxTexture;
	};

}
