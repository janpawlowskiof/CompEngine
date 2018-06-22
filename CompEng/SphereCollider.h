#pragma once

#include "colliderComponent.h"

class SphereCollider : public ColliderComponent
{
public:
	void RecalculateInteria()
	{
		TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");
		RigidbodyComponent* rigidbody = (RigidbodyComponent*)baseObject->GetComponent("Rigidbody");
		if (!rigidbody)
			return;
		interia = glm::mat3(0);
		float x = 1, y = 1, z = 1;
		x = 2 * transform->scale.x;
		y = 2 * transform->scale.y;
		z = 2 * transform->scale.z;

		interia[0][0] = 1.0f / 5.0f * (y*y + z*z);
		interia[1][1] = 1.0f / 5.0f * (x*x + z*z);
		interia[2][2] = 1.0f / 5.0f * (x*x + y*y);
		interia *= rigidbody->GetMass();
	}

	glm::vec3 Support(glm::vec3 D)
	{
		TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");
		glm::vec3 position = transform->position;
		return position + glm::normalize(D) * transform->scale.x;
	}
};