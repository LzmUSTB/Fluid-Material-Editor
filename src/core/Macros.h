#pragma once
#include <iostream>
#include "logger/LogLayer.h"

#include "logger/Log.h"
#define FME_DEBUG_LOG_ERROR(...)	FMEditor::Log::GetLogger()->error(__VA_ARGS__)
#define FME_DEBUG_LOG_WARN(...)		FMEditor::Log::GetLogger()->warn(__VA_ARGS__)
#define FME_DEBUG_LOG_INFO(...)		FMEditor::Log::GetLogger()->info(__VA_ARGS__)
#define FME_DEBUG_LOG_TRACE(...)	FMEditor::Log::GetLogger()->trace(__VA_ARGS__)

#define FME_LOG_ERROR(...)	FMEditor::LogLayer::Get()->AddLog(FMEditor::LogLayer::LOG_ERROR,__VA_ARGS__)
#define FME_LOG_WARN(...)		FMEditor::LogLayer::Get()->AddLog(FMEditor::LogLayer::LOG_WARNING,__VA_ARGS__)
#define FME_LOG_INFO(...)		FMEditor::LogLayer::Get()->AddLog(FMEditor::LogLayer::LOG_INFO,__VA_ARGS__)
#define FME_LOG_TRACE(...)	FMEditor::LogLayer::Get()->AddLog(FMEditor::LogLayer::LOG_TRACE,__VA_ARGS__)

#define FME_DEBUG_ASSERT(x) assert(x)

namespace FMEditor {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}