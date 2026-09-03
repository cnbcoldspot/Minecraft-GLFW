#pragma once
#include <chrono>
#include <thread>

#include "World.h" 

// Waits `timer` milliseconds, then flags every chunk for a mesh rebuild.
void Call_UpdateChunks(World& world, unsigned int timer) {
	std::this_thread::sleep_for(std::chrono::milliseconds(timer));
	world.UpdateChunks();
}