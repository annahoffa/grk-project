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
			this->position += this->velocity * (float)deltaTime;
			this->cameraDistance = glm::length2(this->position - cameraPosition);
		}
	}


}