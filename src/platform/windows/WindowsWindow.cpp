#include "fmepch.h"
#include "WindowsWindow.h"
#include "core/Macros.h"

namespace FMEditor {

	static bool s_GLFWInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			if (!glfwInit()) {
				FME_LOG_ERROR("[WindowsWindow.cpp]: Could not initialize GLFW!");
				FME_DEBUG_LOG_ERROR("[WindowsWindow.cpp]: Could not initialize GLFW!");
				FME_DEBUG_ASSERT(0);
			}

			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_DEPTH_BITS, 24);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		if (m_Window == NULL)
		{
			FME_LOG_ERROR("[WindowsWindow.cpp]: Failed to create GLFW window.");
			FME_DEBUG_LOG_ERROR("[WindowsWindow.cpp]: Failed to create GLFW window.");
			glfwTerminate();
			FME_DEBUG_ASSERT(0);
		}

		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			FME_LOG_ERROR("[WindowsWindow.cpp]: Failed to initialize GLAD");
			FME_DEBUG_LOG_ERROR("[WindowsWindow.cpp]: Failed to initialize GLAD");
			FME_DEBUG_ASSERT(0);
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(false);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	bool WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);

		return !glfwWindowShouldClose(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;

		FME_LOG_INFO("[WindowsWindow.cpp]: VSync: %s", enabled ? "ENABLED" : "DISABLED");
		FME_DEBUG_LOG_INFO("[WindowsWindow.cpp]: VSync: {0}", enabled ? "ENABLED" : "DISABLED");
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	float WindowsWindow::GetFrameTime() const
	{
		return (float)glfwGetTime();
	}

}