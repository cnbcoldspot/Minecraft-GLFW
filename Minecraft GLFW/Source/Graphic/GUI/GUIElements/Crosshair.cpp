#include <chrono>
#include <thread>

#include "Crosshair.h"

Crosshair::Crosshair(GUIMesh* parent, unsigned short textureID) : GUIElement(parent, textureID) {
	mode = CrosshairMode::None;
}

void Crosshair::SetMode(CrosshairMode newMode) {
	mode = newMode;
}

void Crosshair::Toggle() {
	mode = CrosshairMode::None;
}

void Crosshair::Trigger() {
	mode = CrosshairMode::Triggered;
	SetTexture(2);
	fut = std::async(std::launch::async, &Crosshair::Untrigger, this, 50);
}

void Crosshair::Untrigger(int time) {
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
	if(mode == CrosshairMode::Triggered) {
		mode = CrosshairMode::Standard;
		SetTexture(1);
	}
}
