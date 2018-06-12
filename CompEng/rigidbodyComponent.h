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
	float inverseMass = 1.0f;
	float inverseInteria = 1.0f;	//???

	//methods
	void Update();

	std::string type() { return "Rigidbody"; }

	RigidbodyComponent();
	~RigidbodyComponent();
};

