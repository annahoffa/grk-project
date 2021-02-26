#pragma once

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "stb_image.h"
#include <vector>

#include "Shader.h"


namespace grk {

	class Particle {
	public:
		void initialize(glm::vec3 position, glm::vec3 velocity, float lifetime);
		void update(float deltaTime, glm::vec3 cameraPosition);
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetime;
		float cameraDistance;
		bool operator<(const Particle& that) const {
			// Sort in reverse order : far particles drawn first.
			return this->cameraDistance > that.cameraDistance;
		}

	private:
		
	};
}