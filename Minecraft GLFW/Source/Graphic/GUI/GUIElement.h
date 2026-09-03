#pragma once
#include <glm/ext.hpp>

#include "GUIMesh.h"

#define GUI_ATLAS_W 64
#define GUI_ATLAS_H 32

class GUIElement
{
protected:
	unsigned short textureID;
	glm::vec2 position;
	glm::vec2 scale;

	GUIMesh* parent;
public:
	GUIElement(GUIMesh* parent);
	GUIElement(GUIMesh* parent, unsigned short textureID);

	void Push();

	void SetTexture(unsigned short textureID);

	void Center();

	void Move(glm::vec2 val);
	void Move(float x, float y);

	void Scale(glm::vec2 val);
	void Scale(float x, float y);
};

