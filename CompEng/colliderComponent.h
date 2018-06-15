#pragma once

#include "collisionData.h"
#include "transformComponent.h"
#include "rigidbodyComponent.h"
#include "component.h"

class ColliderComponent : public Component
{
public:
	float elasticity = 0.2;
	glm::mat3 interia;

	std::string type() { return "Collider"; }

	virtual glm::mat3 GetLocalInverseInteria()
	{
		return glm::inverse(interia);
	}

	virtual glm::mat3 GetGlobalInverseInteria()
	{
		TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");
		auto rotationMat = transform->GetRotationMatrix();
		return rotationMat * GetLocalInverseInteria() * glm::transpose(rotationMat);
	}

	virtual void RecalculateInteria()
	{
		TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");
		RigidbodyComponent* rigidbody = (RigidbodyComponent*)baseObject->GetComponent("Rigidbody");
		if (!rigidbody)
			return;
		interia = glm::mat3(0);
		float x = 1, y = 1, z = 1;
		x = 2*transform->scale.x;
		y = 2*transform->scale.y;
		z = 2*transform->scale.z;

		interia[0][0] = 1.0f / 12.0f * (y*y + z*z);
		interia[1][1] = 1.0f / 12.0f * (x*x + z*z);
		interia[2][2] = 1.0f / 12.0f * (x*x + y*y);
		interia *= rigidbody->GetMass();
	}
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

	void OnAttach() { RecalculateInteria(); }
	ColliderComponent() {}
	~ColliderComponent() {}
};

