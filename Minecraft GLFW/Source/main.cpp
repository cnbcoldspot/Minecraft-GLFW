/*
	A 64 MB main-thread stack, kept from the original Visual Studio build.
	Linux has no link-time equivalent (the main stack comes from the shell's
	RLIMIT_STACK), but nothing here needs it: chunks and voxels are all
	heap-allocated, and the default 8 MB is plenty.
*/
#ifdef _WIN32
#	pragma comment(linker, "/STACK:67108864")
#	include <Windows.h>
#endif
#include <iostream>
#include <ctime>
#include <future>
#include <filesystem>
#include <system_error>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ErrorHandling.h"

#include "Window/Window.h"
#include "Window/Events.h"
#include "Window/Camera.h"

#include "Graphic/Shader.h"

#include "Graphic/GUI/GUIMesh.h"
#include "Graphic/GUI/GUIElement.h"

#include "Mesh/Mesh.h"

#include "Mesh/BigMesh.h"
#include "Mesh/ShardMesh.h"

#include "World/World.h"
#include "World/ChunkUpdaterCall.h"

#include "Player/Player.h"

#include "Player/PlayerGUI.h"

#include "World/Voxel/VoxelDataBase.h"

using namespace glm;

/*
**********************************************************
*To compile the project, set the Release and x64 settings*
**********************************************************
*/


/*
	Shaders and textures are opened by paths relative to the working
	directory ("Resource/Shader/..."), which used to mean "run it from the
	Visual Studio project directory". Anchor the working directory to the
	executable instead - the build copies Resource/ next to the binary - so
	the game runs the same however it was launched.
*/
static void SetWorkingDirectoryToExecutable() {
	std::error_code error;
	std::filesystem::path executable;

#ifdef _WIN32
	wchar_t buffer[MAX_PATH];
	DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
	if(length == 0 || length == MAX_PATH) {
		// [main] Could not locate the executable, keep the current directory
		return;
	}
	executable = std::filesystem::path(buffer, buffer + length);
#else
	executable = std::filesystem::read_symlink("/proc/self/exe", error);
	if(error) {
		// [main] No /proc, keep the current directory
		return;
	}
#endif

	std::filesystem::current_path(executable.parent_path(), error);
	if(error) {
		std::cerr << "[main] Failed to enter " << executable.parent_path() << std::endl;
	}
}

int main() {

	SetWorkingDirectoryToExecutable();

	srand(time(0));
	std::ios_base::sync_with_stdio(0);

	//Window initialization
	Window::Initialize("Hello world", true);
	Events::Initialize();

	/*
		The frame rate is uncapped, which is what the F3 counter is there to
		show. Window::SetVSync(true) locks it to the display refresh rate
		instead - quieter, and no tearing.
	*/

	//Initializatioo of main Shader
	Shader* shader = CreateShaderProgram("Resource/Shader/mainVertex.glsl", "Resource/Shader/mainFragment.glsl");
	if (shader == nullptr) {
		std::cerr << "[main] Failed to load main shader" << std::endl;
		Window::Terminate();
		return -1;
	}

	//Initializatioo of gui Shader
	Shader* guiShader = CreateShaderProgram("Resource/Shader/guiShaderVertex.glsl", "Resource/Shader/guiShaderFragment.glsl");
	if (guiShader == nullptr) {
		std::cerr << "[main] Failed to load gui shader" << std::endl;
		Window::Terminate();
		return -1;
	}

	//Initializatioo of texture atlas
	Texture* textureAtlas = CreateTexture("Resource/Textures/TextureAtlas.png");
	if (textureAtlas == nullptr) {
		std::cerr << "[main] Failed to load texture atlas" << std::endl;
		Window::Terminate();
		return -1;
	}

	//Initializatioo of gui atlas
	Texture * guiTextureAtlas = CreateTexture("Resource/Textures/Crosshair.png");
	if(guiTextureAtlas == nullptr) {
		std::cerr << "[main] Failed to load gui texture atlas" << std::endl;
		Window::Terminate();
		return -1;
	}


	glClearColor(0.24f, 0.47f, 0.69f, 1);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat4 Model(1.f);
	Model = translate(Model, vec3(0.5f, 0, 0));

	Events::ToogleCursor();

	World world(textureAtlas, 1);

	PlayerGUI pGUI(guiTextureAtlas, (float)Window::Width / (float)Window::Height);

	Player Steve(&world, &pGUI);


	// launching asynchronous task to update all chunks once after 3000ms
	auto fut = std::async(std::launch::async, &Call_UpdateChunks, std::ref(world), 3000);

	//Main loop
	while (!Window::WindowShouldClose()) {

		Steve.Update();

		shader->Bind();
		shader->UniformMatrix("projview", Steve.getCamera()->GetProjection() * Steve.getCamera()->GetView());
		shader->Unbind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw here
		world.Draw(shader);
		Steve.DrawGUI(guiShader);

		Events::PullEvents();
		Window::SwapBuffers();
	}



	delete shader;
	delete guiShader;
	delete textureAtlas;

	Window::Terminate();

	return 0;
}
