#pragma once

#include "collisionData.h"
#include "transformComponent.h"
#include "component.h"

class ColliderComponent : public Component
{
public:
	float elasticity = 0.35;

	std::string type() { return "Collider"; }

	virtual glm::vec3 Support(glm::vec3 D)	//box by default
	{
		glm::vec4 boxPoints[8] = {
			glm::vec4(-1, 1, 1, 1),
			glm::vec4(1, 1, 1, 1),
			glm::vec4(1, -1, 1, 1),
			glm::vec4(-1, -1, 1, 1),
			glm::vec4(-1, 1, -1, 1),
			glm::vec4(1, 1, -1, 1),
			glm::vec4(1, -1, -1, 1),
			glm::vec4(-1, -1, -1, 1) };

		TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");
		glm::mat4 model = transform->GetModelMatrix();

		glm::vec3 points[8];
		for (int i = 0; i < 8; ++i)
			points[i] = model * boxPoints[i];

		glm::vec3 furthestPoint = points[0];
		float furthestDistance = glm::dot(D, points[0]);
		for (int i = 1; i < 8; ++i)
		{
			float distance = glm::dot(D, points[i]);
			if (distance > furthestDistance)
			{
				furthestDistance = distance;
				furthestPoint = points[i];
			}
		}
		return furthestPoint;
	}

	ColliderComponent() {}
	~ColliderComponent() {}
};

