#pragma once

#include "colliderComponent.h"

class SphereCollider : public ColliderComponent
{
public:
	glm::vec3 Support(glm::vec3 D)
	{
		glm::vec3 position = ((TransformComponent*)baseObject->GetComponent("Transform"))->position;
		return position + glm::normalize(D) * 0.5;
	}
};