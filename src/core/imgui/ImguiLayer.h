#pragma once
#include "core/Layer.h"

namespace FMEditor {
	class ImguiLayer : public Layer
	{
	public:
		ImguiLayer();
		~ImguiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		//virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = true;
	};
}


