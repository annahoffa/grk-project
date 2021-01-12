#include "transformations.h"

namespace T
{
	glm::mat4 orbitalSpeed(float angle)
	{
		float time = glutGet(GLUT_ELAPSED_TIME) / 10000.0f;

		glm::mat4 rotation;
		rotation = glm::rotate(rotation, time * glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		return rotation;
	}

	glm::mat4 scaling(float size)
	{
		glm::mat4 scal = glm::scale(glm::vec3(size, size, size));
		return scal;
	}

	glm::mat4 moonRotation(float angle, float deviation)
	{
		float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		glm::mat4 rotation;
		rotation = glm::rotate(rotation, time * glm::radians(angle), glm::vec3(0.0f, 0.1f, deviation));

		return rotation;
	}
}