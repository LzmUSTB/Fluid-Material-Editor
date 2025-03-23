#include "fmepch.h"
#include "Log.h"
#include "core/Macros.h"


namespace FMEditor {
	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt("FMEditor_Log");
		s_Logger->set_level(spdlog::level::trace);
		FME_DEBUG_LOG_INFO("[Log.cpp]: spdlog initialized.");
		FME_LOG_INFO("[Log.cpp]: spdlog initialized.");
	}
}
