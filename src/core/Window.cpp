#include "fmepch.h"
#include "Window.h"

#ifdef FME_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace FMEditor {
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef FME_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		FME_LOG_ERROR("FME only support Windows!");
		return nullptr;
#endif
	}
}