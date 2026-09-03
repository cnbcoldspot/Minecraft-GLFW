#pragma once

#include <glm/glm.hpp>

class Entity {
public:
    /*
        These are default-initialized on purpose. glm::vec3's default
        constructor leaves its components uninitialized, and Player's
        constructor assigns only some of the members below - Velocity and
        m_acceleration were never set, so the first physics step read stack
        garbage. A garbage Velocity threw Position to around 1e30 on frame one,
        and from there the float loops in Player::CollisionTest() could no
        longer advance (0.1f is far below the ULP at that magnitude), so the
        game hung on the second frame. It only happened when the stack
        happened to hold an unlucky value, which made it look intermittent.
    */
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 View = glm::vec3(0.0f);

	glm::vec3 Velocity = glm::vec3(0.0f);
	glm::vec3 m_acceleration = glm::vec3(0.0f);

	float LastTime = 0.0f;
	float Delta = 0.0f;

	bool isOnGround = false;
};
