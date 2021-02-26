#include <iostream>
#include "particle_emitter.h"

namespace grk {
	
	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};

	static const int MaxParticles = 500;
	static GLfloat* g_particle_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particle_color_data = new GLubyte[MaxParticles * 4];


	void ParticleEmitter::initialize(Shader& shader, GLuint texture, glm::vec3 initialVelocity, float size, glm::vec3 translationOffset) {
		for (int i = 0; i < MaxParticles; i++) {
			Particle p;
			p.initialize(glm::vec3 (0), glm::vec3(0), -1);
			this->particles.push_back(p);
		}
		this->shader = &shader;
		this->texture = texture;
		this->initialVelocity = initialVelocity;
		this->size = size;
		this->translationOffset = translationOffset;

		// Vertex shader
		CameraRight_worldspace_ID = shader.getUniform("CameraRight_worldspace");
		CameraUp_worldspace_ID = shader.getUniform("CameraUp_worldspace");
		ViewProjMatrixID = shader.getUniform("VP");

		// fragment shader
		textureID = shader.getUniform("myTextureSampler");

		glGenBuffers(1, &billboard_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &particles_position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		// The VBO containing the colors of the particles
		glGenBuffers(1, &particles_color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	}

	void ParticleEmitter::update(glm::vec3 emitterPosition, glm::quat emitterRotation, float deltaTime, glm::vec3 cameraPosition, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
		if (!enabled) {
			return;
		}

		createParticles(emitterPosition, emitterRotation);
		int particlesCount = 0;

		for (int i = 0; i < particles.size(); i++) {
			Particle* particle = &particles[i];
			if (particle->lifetime > 0) {
				particle->update(deltaTime, cameraPosition);

				// Fill the GPU buffer
				g_particle_position_size_data[4 * particlesCount + 0] = particle->position.x;
				g_particle_position_size_data[4 * particlesCount + 1] = particle->position.y;
				g_particle_position_size_data[4 * particlesCount + 2] = particle->position.z;
				g_particle_position_size_data[4 * particlesCount + 3] = size;

				g_particle_color_data[4 * particlesCount + 0] = 255;
				g_particle_color_data[4 * particlesCount + 1] = 200;
				g_particle_color_data[4 * particlesCount + 2] = 60;
				g_particle_color_data[4 * particlesCount + 3] = 128;

				particlesCount++;
			} else {
				particle->cameraDistance = -1.0f;
			}

		}
		sortParticles();
		std::cout << particlesCount << std::endl;

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, g_particle_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, g_particle_color_data);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader->use();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(textureID, 0);

		glUniform3f(CameraRight_worldspace_ID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

		glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &viewProjectionMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

		// This is equivalent to :
		// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
		// but faster.
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void ParticleEmitter::enable() {
		this->enabled = true;
	}

	void ParticleEmitter::disable() {
		this->enabled = false;
	}

	void printVector(glm::vec3 v) {
		std::cout << "{x:" << v.x << "y:" << v.y << "z:" << v.z << "}";
	}

	void ParticleEmitter::createParticles(glm::vec3 initialPosition, glm::quat rotation) {
		Particle* newParticle = this->findNextParticle();
		float initialVelocityLength = glm::length(initialVelocity) * 2;
		glm::vec3 variation;
		variation.x = (rand() % 100) / 100.0f * initialVelocityLength / 2 - initialVelocityLength / 4;
		variation.y = (rand() % 100) / 100.0f * initialVelocityLength / 2 - initialVelocityLength / 4;
		variation.z = (rand() % 100) / 100.0f * initialVelocityLength / 2 - initialVelocityLength / 4;
		newParticle->initialize(initialPosition + (translationOffset * rotation), (initialVelocity + variation) * rotation, 500);
		//newParticle->initialize(glm::vec3 (0), glm::vec3 (0.01), 5000);
	}

	void ParticleEmitter::sortParticles() {
		std::sort(&particles[0], &particles[MaxParticles]);
	}

	Particle * ParticleEmitter::findNextParticle() {
		this->lastParticle++;
		while (this->particles[this->lastParticle].lifetime > 0) {
			this->lastParticle = (this->lastParticle + 1) % this->particles.size();
		}
		return &(this->particles[lastParticle]);
	}
}