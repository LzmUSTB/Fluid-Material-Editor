#include "fmepch.h"
#include "WindowsWindow.h"
#include "Core/Log.h"

namespace FMEditor {

	static bool s_GLFWInitialized = false;
	//static uint8_t s_GLFWWindowCount = 0;

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

		//PE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			if (!glfwInit())
				FME_ERROR("Could not initialize GLFW!");
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		if (m_Window == NULL)
		{
			FME_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return;
		}
		//m_Context = new OpenGLContext(m_Window);
		//m_Context->Init();

		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			FME_ERROR("CFailed to initialize GLAD");
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(false);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);

		// TODO: temporary
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}