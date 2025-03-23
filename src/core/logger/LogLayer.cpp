#include "fmepch.h"
#include "LogLayer.h"
#include "core/Macros.h"

namespace FMEditor {


	LogLayer* LogLayer::s_Instance = nullptr;

	LogLayer* LogLayer::Get()
	{
		if (s_Instance == nullptr) {
			s_Instance = new LogLayer();
		}
		return s_Instance;
	}

	LogLayer::LogLayer() :Layer("LogLayer") {
	}

	void LogLayer::OnImguiRender()
	{
		ImGui::Begin("Log");

		// ui header
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll); ImGui::SameLine();
		ImGui::Checkbox("Error", &m_ShowError); ImGui::SameLine();
		ImGui::Checkbox("Warning", &m_ShowWarning); ImGui::SameLine();
		ImGui::Checkbox("Info", &m_ShowInfo); ImGui::SameLine();
		ImGui::Checkbox("Trace", &m_ShowTrace);
		ImGui::Separator();

		// ui body
		const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		for (const LogEntry& item : m_LogItems) {
			if (item.level == LOG_ERROR && !m_ShowError)continue;
			if (item.level == LOG_INFO && !m_ShowInfo)continue;
			if (item.level == LOG_WARNING && !m_ShowWarning)continue;
			if (item.level == LOG_TRACE && !m_ShowTrace)continue;

			if (!m_Filter.PassFilter(item.msg.c_str()))
				continue;

			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			switch (item.level) {
			case LOG_ERROR: {
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
				ImGui::TextColored(color, "[ERROR]: ");
				break;
			}
			case LOG_INFO: {
				color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				ImGui::TextColored(color, "[INFO]: ");
				break;
			}
			case LOG_TRACE: {
				color = ImVec4(0.3f, 1.0f, 0.4f, 1.0f);
				ImGui::TextColored(color, "[TRACE]: ");
				break;
			}
			case LOG_WARNING: {
				color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
				ImGui::TextColored(color, "[WARNING]: ");
				break;
			}
			}
			ImGui::SameLine();
			ImGui::TextUnformatted(item.msg.c_str());
		}
		ImGui::PopStyleVar();

		if (m_ScrollToBottom && m_AutoScroll)
			ImGui::SetScrollHereY(1.0f);
		m_ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::Separator();

		// ui bottom
		m_Filter.Draw("Search", -100.0f);

		ImGui::End();
	}

	void LogLayer::AddLog(LogLevel level, const char* fmt, ...)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		m_LogItems.push_back({ level,buf });
		if (m_AutoScroll)
			m_ScrollToBottom = true;
	}

	void LogLayer::OnAttach()
	{
	}

}
