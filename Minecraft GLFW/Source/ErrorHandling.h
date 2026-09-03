#pragma once
#include <GL/glew.h>
#include <iostream>

#include "Utility/Mexception.h"

/*
	Portable debug break.
	MSVC provides __debugbreak() as an intrinsic; on other toolchains
	we fall back to the compiler builtin, and finally to SIGTRAP.
*/
#if defined(_MSC_VER)
#	define DEBUGBREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#	define DEBUGBREAK() __builtin_trap()
#else
#	include <csignal>
#	define DEBUGBREAK() raise(SIGTRAP)
#endif

#define CONSOLEMSG(x) std::cerr << x << std::endl;

#define ASSERT(x) if (!(x)) DEBUGBREAK();
 
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall());\


static void GLClearError()
{
	while(glGetError() != GL_NO_ERROR); 
}

static bool GLLogCall()
{
	while(GLenum error = glGetError())
	{
		std::cerr << "[OpenGL Error] ( " << error << " )\n";
		return false;
	}
	return true;
}
