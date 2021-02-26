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
#include "particle.h"


namespace grk {

	class ParticleEmitter {
	public:
		ParticleEmitter() {}
		void update(glm::vec3 emitterPosition, glm::quat quaternionRotation, float deltaTime, glm::vec3 cameraPosition, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
		void enable();
		void disable();
		void initialize(Shader& shader, GLuint texture, glm::vec3 initialVelocity, float size, glm::vec3 translationOffset);

	private:
		std::vector< Particle > particles;
		int lastParticle = -1;
		glm::vec3 particleColor;
		glm::vec3 initialVelocity;
		bool enabled = true;
		void createParticles(glm::vec3 initialPosition, glm::quat rotation);
		void sortParticles();
		Shader* shader;
		Particle * findNextParticle();
		GLuint billboard_vertex_buffer;
		GLuint particles_position_buffer;
		GLuint particles_color_buffer;
		GLuint texture;
		GLuint textureID;
		GLuint CameraRight_worldspace_ID;
		GLuint CameraUp_worldspace_ID;
		GLuint ViewProjMatrixID;
		float size;
		glm::vec3 translationOffset;
	};
}