#pragma once
#include <iostream>
#include "Log.h"

#define FME_ERROR(...)	FMEditor::Log::GetLogger()->error(__VA_ARGS__)
#define FME_WARN(...)	FMEditor::Log::GetLogger()->warn(__VA_ARGS__)
#define FME_INFO(...)	FMEditor::Log::GetLogger()->info(__VA_ARGS__)
#define FME_TRACE(...)	FMEditor::Log::GetLogger()->trace(__VA_ARGS__)