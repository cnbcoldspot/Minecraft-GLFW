#pragma once

//GLEW in Satic Mode
#ifndef GLEW_STATIC
#	define GLEW_STATIC
#endif

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
	static int Width;
	static int Height;

	static GLFWmonitor* monitor;
	static GLFWwindow* window;

	//Window initialization method
	static int Initialize(const char* title, bool fullscreen);
	
	//Window killing method
	static void Terminate();
	
	//Method for checking if a window is open
	static bool WindowShouldClose();

	static bool WindowIsFocused();
	
	//Window close method
	static void SetWindowShouldClose(bool flag);
	
	//Method for changing display buffers
	static void SwapBuffers();

	//Frame rate limiting. Off means the render loop runs as fast as it can;
	//on locks it to the display refresh rate.
	static void SetVSync(bool enabled);

	static void SetCursorMode(int mode);
};

