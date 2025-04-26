#pragma once
#include "core/Layer.h"
#include "Renderer.h"
#include "imgui.h"
#include "entt/entity/registry.hpp"
#include "core/Macros.h"
#include "renderer/Shader.h"
#include "renderer/FrameBuffer.h"
#include "renderer/Texture.h"

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
		uint32_t SwitchImage(uint32_t option);
		void ApplyFilter();
		void CalculateNormal();
		void FinalProcess();

		bool m_IsFirstFrame = true;
		Scope<Renderer> m_Renderer;
		ImVec2 m_WindowSize;
		Scope<FrameRateCounter> m_Counter;
		uint32_t m_ResolutionWidth;
		uint32_t m_ResolutionHeight;

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
		Scope<Shader> m_billboardShader;
		Scope<Shader> m_pointShader;
		Scope<Shader> m_particleThicknessShader;
		Scope<Shader> m_particleDepthShader;
		Scope<Shader> m_narrowRangeFilterShader;
		Scope<Shader> m_normalMapShader;
		Scope<Shader> m_finalProcessShader;
		Scope<Shader> m_gaussianFilterShader;
		Scope<Shader> m_linearDepthShader;
		Scope<Shader> m_2dFilterShader;

		Scope<FrameBuffer> m_MainFramebuffer;
		Scope<Texture> m_RenderTexture;
		Scope<FrameBuffer> m_SceneFrameBuffer;
		Scope<Texture> m_SceneTexture;
		//Scope<FrameBuffer> m_ThicknessBuffer;
		//Scope<Texture> m_ThicknessTexture;
		Scope<Texture> m_DepthTexture;
		Scope<FrameBuffer> m_NormalMapBuffer;
		Scope<Texture> m_NormalMapTexture;

		Scope<FrameBuffer> m_PingpongFBO_Thickness[2];
		Scope<FrameBuffer> m_PingpongFBO_Depth[2];
		Scope<Texture> m_PingpongTexture_Thickness[2];
		Scope<Texture> m_PingpongTexture_Depth[2];

		unsigned int m_skyboxTexture;

		// adjustable parameter
		float m_particleSize = 0.025f;
		int m_textureShown = 0;
		int m_filterIterations = 6;
		float m_filterRange = 0.f;
		float m_filterOffset = 0.f;
		float m_absorption = 0.02f;
		float refractOffsetAmount = 0.05f;
		float m_color[3] = {0};
	};

}
