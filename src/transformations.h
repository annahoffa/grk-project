#pragma once

#include "freeglut.h"
#include "ext.hpp"

namespace T
{
	glm::mat4 orbitalSpeed(float angle);
    glm::mat4 scaling(float size);
    glm::mat4 moonRotation(float angle, float deviation);
}
