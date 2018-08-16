#pragma once

#include "component.h"
#include "baseObject.h"
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum LightType
{
	LIGHT_DIRECTIONAL = 0,
	LIGHT_POINT = 1,
	LIGHT_SPOT = 2
};

class LightComponent : public Component
{
public:
	int lightType;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 direction;

	float constant;
	float linear;
	float quadratic;

	LightComponent(glm::vec3 dir, glm::vec3 am, glm::vec3 di, glm::vec3 sp)
	{
		direction = dir;
		ambient = am;
		diffuse = di;
		specular = sp;

		lightType = LIGHT_DIRECTIONAL;
	}

	LightComponent(glm::vec3 am, glm::vec3 di, glm::vec3 sp, float co, float li, float qu)
	{
		ambient = am;
		diffuse = di;
		specular = sp;
		constant = co;
		linear = li;
		quadratic = qu;

		lightType = LIGHT_POINT;
	}
};