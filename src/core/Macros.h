#pragma once
#include <iostream>
#include "Log.h"

#define FME_ERROR(...)	FMEditor::Log::GetLogger()->error(__VA_ARGS__)
#define FME_WARN(...)	FMEditor::Log::GetLogger()->warn(__VA_ARGS__)
#define FME_INFO(...)	FMEditor::Log::GetLogger()->info(__VA_ARGS__)
#define FME_TRACE(...)	FMEditor::Log::GetLogger()->trace(__VA_ARGS__)

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