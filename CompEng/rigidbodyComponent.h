#pragma once
#include "component.h"
class RigidbodyComponent : public Component
{
private:

public:
	//per physics frame data
	glm::vec3 force;
	glm::vec3 torque;

	//propeties
	glm::vec3 velocity;
	glm::vec3 angularVelocity;
	float mass = 1.0f;

	//methods
	void Update();	//override
	float GetMass() { return mass; };
	float GetInverseMass() { return 1.0/mass; };

	std::string type() { return "Rigidbody"; }

	RigidbodyComponent();
	~RigidbodyComponent();
};

