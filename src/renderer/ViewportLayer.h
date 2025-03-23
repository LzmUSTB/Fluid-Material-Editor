#pragma once
#include "core/Layer.h"
#include "Renderer.h"
#include "imgui.h"

namespace FMEditor {

	class ViewportLayer : public Layer
	{
	public:
		ViewportLayer();
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnImguiRender() override;

	private:
		bool m_IsFirstFrame = true;
		Scope<Renderer> m_Renderer;
		ImVec2 m_WindowSize;
		ImTextureID m_TextureID;
	};

}
