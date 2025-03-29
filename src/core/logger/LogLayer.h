#pragma once
#include "core/Layer.h"
#include "imgui.h"

namespace FMEditor {

	class LogLayer : public Layer
	{
	public:
		enum LogLevel {
			LOG_INFO,
			LOG_WARNING,
			LOG_ERROR,
			LOG_TRACE
		};

		struct LogEntry {
			LogLevel level;
			std::string msg;
		};

		~LogLayer();

		// from Layer
		void OnAttach() override;
		//void OnDetach() override;
		void OnImguiRender() override;

		// original
		void AddLog(LogLevel level, const char* fmt, ...);
		static LogLayer* Get();
	private:
		LogLayer();
		static LogLayer* s_Instance;

		std::vector<LogEntry> m_LogItems;
		// imgui 
		bool m_AutoScroll = true;
		bool m_ScrollToBottom = false;
		bool m_ShowError = true;
		bool m_ShowInfo = true;
		bool m_ShowWarning = true;
		bool m_ShowTrace = true;
		ImGuiTextFilter  m_Filter;
		ImVector<int> m_LineOffsets;
	};

}

