#include "stdafx.h"
#include "rigidbodyComponent.h"
#include "transformComponent.h"
#include "baseObject.h"

#define dT 0.02f
#define DAMPEN 0.2

void RigidbodyComponent::Update()
{
	TransformComponent* transform = (TransformComponent*)baseObject->GetComponent("Transform");

	glm::vec3 acceleration = force * inverseMass;
	transform->position += velocity * dT + acceleration * dT * dT / 2.0;
	velocity += acceleration * dT;

	glm::quat qx = glm::angleAxis(dT * angularVelocity.x, glm::vec3(1, 0, 0));
	glm::quat qy = glm::angleAxis(dT * angularVelocity.y, glm::vec3(0, 1, 0));
	glm::quat qz = glm::angleAxis(dT * angularVelocity.z, glm::vec3(0, 0, 1));

	transform->RotateGlobal(qx * qy * qz);

	velocity *= (1.0 - dT * DAMPEN);
	angularVelocity *= (1.0 - dT * DAMPEN);

	//std::cout << velocity.y << std::endl;
	force = glm::vec3(0);
}

RigidbodyComponent::RigidbodyComponent()
{
}


RigidbodyComponent::~RigidbodyComponent()
{
}
