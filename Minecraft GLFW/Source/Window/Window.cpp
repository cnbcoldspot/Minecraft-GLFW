#include "Window.h"

#include "../ErrorHandling.h"

GLFWwindow* Window::window;
GLFWmonitor* Window::monitor;
int Window::Width = 0;
int Window::Height = 0;

int Window::Initialize(const char* title, bool fullscreen) {
	//Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	monitor = glfwGetPrimaryMonitor();
	if(monitor == nullptr) {
		std::cout << "[Window] Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		DEBUGBREAK();
	}
	int m_width = 800, m_height = 600;
	int xpos = 0, ypos = 0;

	//Create a window
	if(fullscreen) {
		/*
			Fullscreen has to be sized from the monitor's video mode. The work
			area excludes taskbars and panels, so using it here would ask for a
			resolution the monitor has no mode for.
		*/
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if(mode == nullptr) {
			std::cout << "[Window] Failed to query the monitor video mode" << std::endl;
			glfwTerminate();
			DEBUGBREAK();
		}
		m_width = mode->width;
		m_height = mode->height;
		window = glfwCreateWindow(m_width, m_height, title, monitor, nullptr);
	}
	else {
		glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &m_width, &m_height);
		window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
	}
	if (window == nullptr) {
		std::cout << "[Window] Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		DEBUGBREAK();
	}
	glfwMakeContextCurrent(window);

	/*
		Set the swap interval explicitly. Nothing here used to call
		glfwSwapInterval(), so the frame rate depended on the driver default -
		uncapped on Windows, but locked to the refresh rate by Mesa on Linux.
	*/
	SetVSync(false);

	//Initializing GLEW
	glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();
	/*
		Under Wayland GLFW hands us an EGL context and GLEW's GLX probe fails,
		even though every entry point it resolved is valid. That one error is
		not fatal; anything else is.
	*/
	if (glewStatus != GLEW_OK && glewStatus != GLEW_ERROR_NO_GLX_DISPLAY) {
		std::cout << "[Window] Failed to initialize GLEW: " << glewGetErrorString(glewStatus) << std::endl;
		DEBUGBREAK();
	}
	/*
		Take the size from the framebuffer rather than from what we asked for:
		with GLFW_SCALE_TO_MONITOR the two differ on HiDPI displays, and a
		fullscreen request can be satisfied by a nearby video mode.
	*/
	glfwGetFramebufferSize(window, &m_width, &m_height);
	glViewport(0, 0, m_width, m_height);

	Window::Width = m_width;
	Window::Height = m_height;

	return 0;
}

void Window::Terminate() {
	glfwTerminate();
}

bool Window::WindowShouldClose() {
	return glfwWindowShouldClose(window);
}

bool Window::WindowIsFocused() {
	return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void Window::SetWindowShouldClose(bool flag) {
	glfwSetWindowShouldClose(window, flag);
}

void Window::SwapBuffers() {
	glfwSwapBuffers(window);
}

void Window::SetCursorMode(int Mode) {
	glfwSetInputMode(window, GLFW_CURSOR, Mode);
}

void Window::SetVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0);
}
