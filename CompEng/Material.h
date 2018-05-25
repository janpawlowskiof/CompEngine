#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Material
{
	glm::vec3 ambient = glm::vec3(0.9, 0.4, 0.4);
	glm::vec3 diffuse = glm::vec3(0.9, 0.4, 0.4);
	glm::vec3 specular = glm::vec3(1, 1, 1);
	float shininess = 32;
};