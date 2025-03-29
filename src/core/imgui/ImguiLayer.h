#pragma once
#include "core/Layer.h"

namespace FMEditor {
	class ImguiLayer : public Layer
	{
	public:
		ImguiLayer();
		~ImguiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		//virtual void OnEvent(Event& e) override;
		void OnImguiRender() override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();

		//uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = true;
	};
}


