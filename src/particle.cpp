#include <iostream>
#include "particle.h"

namespace grk {

	void Particle::initialize(glm::vec3 position, glm::vec3 velocity, float lifetime) {
		this->position = position;
		this->velocity = velocity;
		this->lifetime = lifetime;
	}

	void Particle::update(float deltaTime, glm::vec3 cameraPosition) {
		this->lifetime -= deltaTime;
		if (this->lifetime > 0.0f) {
			// Simulate simple physics : gravity only, no collisions
			/*p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;*/
			this->position += this->velocity * (float)deltaTime;
			this->cameraDistance = glm::length2(this->position - cameraPosition);
		}
	}


}